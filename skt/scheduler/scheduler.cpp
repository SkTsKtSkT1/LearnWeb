#include "scheduler.h"
#include "../log/log.h"
#include "../macro/macro.h"
#include "skt/hook/hook.h"

namespace skt{
static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_sheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name)
    :m_name(name){
    SKT_ASSERT(threads > 0);
    if(use_caller){
        skt::Fiber::GetThis(); //如果线程没有协程 则初始化一个主协程
        --threads;
        SKT_ASSERT(GetThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        skt::Thread::SetName(m_name);

        t_sheduler_fiber = m_rootFiber.get();
        m_rootThread = skt::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }else{
        m_rootThread = -1;
    }
    m_threadCount = threads;

}

Scheduler::~Scheduler() {
    SKT_ASSERT(m_stopping);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
}

Scheduler *Scheduler::GetThis() {
    return t_scheduler;
}

Fiber *Scheduler::GetMainFiber() {
    return t_sheduler_fiber;
}

void Scheduler::start() {
    MutexType::Lock lock(m_mutex);
    if(!m_stopping){
        return;
    }
    m_stopping = false;
    SKT_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);
    for(size_t i = 0; i < m_threadCount; ++i){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();

//    if(m_rootFiber){
////        m_rootFiber->swapIn();
//        m_rootFiber->call();
//        SKT_LOG_INFO(g_logger) << " call out " << m_rootFiber->getState();
//    }
}

void Scheduler::stop() {
    m_autoStop = true;
    if(m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)){
        //SKT_LOG_INFO(g_logger) << this << "stopped";

        SKT_LOG_DEBUG(g_logger) << "scheduler stopped, fiber id=" << t_sheduler_fiber->getId();
        m_stopping = true;

        if(stopping()){
            return;
        }
    }

    //bool exit_on_this_fiber = false;
    if(m_rootThread != -1){
        //this is use_caller thread;
        SKT_ASSERT(GetThis() == this);//要在这个线程中执行stop
    }else{
        SKT_ASSERT(GetThis() != this);
    }
    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i){
        tickle();//唤醒线程
    }
    if(m_rootFiber){
        tickle();
    }

    if(m_rootFiber){
//        while(!stopping()){
//            if(m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::EXCEPT){
//                m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
//                SKT_LOG_INFO(g_logger) << "root fiber is term, reset";
//                t_fiber = m_rootFiber.get(); //
//            }
//            m_rootFiber->call();
//         }
        if(!stopping()){
            m_rootFiber->call();
        }
    }
//    if(stopping()){
//        return;
//    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }
    for(auto& i :thrs){
        i->join();
    }
//    if(exit_on_this_fiber){
//        return ;
//    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

void Scheduler::run() {
    SKT_LOG_DEBUG(g_logger) << "run and enable hook";
    set_hook_enable(true);
    setThis();

    if(skt::GetThreadId() != m_rootThread){
        t_sheduler_fiber = Fiber::GetThis().get();
    }
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    FiberAndThread ft;

    while(true){
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()){
                if(it->thread != -1 && it->thread != skt::GetThreadId()){ //if a work is defined in a thread but not equal this thread;
                    ++it;
                    tickle_me = true;
                    continue;
                }

                SKT_ASSERT(it->fiber || it->cb);
                //if the task is a fiber
                if(it->fiber && it->fiber->getState() == Fiber::EXEC){
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
            tickle_me |= it != m_fibers.end();
        }
        if(tickle_me){
            tickle();
        }

        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT)){ // before is || and the bug caused,如果是或，那么右边永远为真，也就是说不论什么fiber都会进入
            ft.fiber -> swapIn();
            --m_activeThreadCount;
            if(ft.fiber->getState() == Fiber::READY){
                schedule(ft.fiber);
            }else if(ft.fiber->getState() != Fiber::TERM && ft.fiber->getState() != Fiber::EXCEPT){ // before is || and the bug caused,如果是或，那么右边永远为真，也就是说不论什么fiber都会进入，因此都会转为Hold状态
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        }else if(ft.cb){
            if(cb_fiber){
                cb_fiber->reset(ft.cb);
            }else{
                cb_fiber.reset(new Fiber(ft.cb));
                //ft.fiber = nullptr;
            }
            ft.reset();
            cb_fiber->swapIn();
            //执行完协程回来
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY){
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM){
                cb_fiber->reset(nullptr);
            }else{ //if(cb_fiber->getState() != Fiber::TERM){
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        }else{
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::TERM){
                SKT_LOG_INFO(g_logger) << "idle fiber term";
                //tickle();
                break;
                //continue;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();

            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT){
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }

}

void Scheduler::tickle() {
    SKT_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    SKT_LOG_INFO(g_logger) << "idle";
    while(!stopping()){
        skt::Fiber::YieldToHold();
    }
}

}

#include "scheduler.h"
#include "../log/log.h"
#include "../macro/macro.h"

namespace skt{
static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name)
    :m_name(name){
    SKT_ASSERT(threads > 0);
    if(use_caller){
        skt::Fiber::GetThis(); //如果线程没有协程 则初始化一个主协程
        --threads;
        SKT_ASSERT(GetThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this)));
        skt::Thread::SetName(m_name);

        t_fiber = m_rootFiber.get();
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
    return t_fiber;
}

void Scheduler::Start() {
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
}

void Scheduler::Stop() {
    m_autoStop = true;
    if(m_rootFiber && m_threadCount == 0 && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)){
        SKT_LOG_INFO(g_logger) << this << "stopped";
        m_stopping = true;

        if(stopping()){
            return;
        }
    }

    bool exit_on_this_fiber = false;
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

    if(stopping()){
        return;
    }
    if(exit_on_this_fiber){
        return ;
    }
}

void Scheduler::setThis() {
    t_scheduler = this;
}

void Scheduler::run() {
    setThis();

    if(skt::GetThreadId() != m_rootThread){
        t_fiber = Fiber::GetThis().get();
    }
    //Fiber::ptr idle_fiber(new Fiber());
}

void Scheduler::tickle() {

}

bool Scheduler::stopping() {
    return false;
}

}

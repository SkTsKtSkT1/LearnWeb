#include "fiber.h"
#include "../config/config.h"
#include "../macro/macro.h"
#include "../log/log.h"
#include "../scheduler/scheduler.h"
#include "atomic"

namespace skt{
static Logger::ptr g_logger = SKT_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};
//https://zhuanlan.zhihu.com/p/77585472
static thread_local Fiber* t_fiber = nullptr; //该线程下的某一fiber
//static thread_local std::shared_ptr<Fiber::ptr> t_threadFiber = nullptr;

static thread_local Fiber::ptr t_threadFiber = nullptr; //store the main fiber

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1204, "fiber stack size");

class MallocStackAllocator{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;


uint64_t Fiber::GetFiberId() {
    if(t_fiber){
        return t_fiber->getId();
    }else{
        return 0;
    }
}

//this is the main fiber
Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);
    if(getcontext(&m_ctx)){
        SKT_ASSERT2(false, "getcontex");
    }

    ++s_fiber_count;

    SKT_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

//this is the real fiber and need the stack size;
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        SKT_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    if(!use_caller){
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }else{
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    SKT_LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack){
        //SKT_LOG_DEBUG(g_logger) << "~fiber id = " << m_id;
        SKT_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    }else{
        SKT_ASSERT(!m_cb);
        SKT_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }

    SKT_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id << " total= " << s_fiber_count;
}

Fiber::ptr Fiber::GetThis() {
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    SKT_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}


void Fiber::SetThis(skt::Fiber *f) {
    t_fiber = f;
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    SKT_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

void Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    SKT_ASSERT(cur->m_state == EXEC);
    //cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::TotalFibers() {
    return s_fiber_count;
}

void Fiber::reset(std::function<void()> cb) {
    SKT_ASSERT(m_stack);
    SKT_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);

    m_cb = cb;
    //m_cb = std::move(cb);
    if(getcontext(&m_ctx)){
        SKT_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void Fiber::call(){
    SetThis(this); //当前的协程
    m_state = EXEC;
    //SKT_LOG_DEBUG(g_logger) << getId();
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)){
        SKT_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        SKT_ASSERT2(false, "swapcontext");
    }
}

//一定是子协程，切换到当前协程执行
void Fiber::swapIn() {
    SetThis(this); //当前的协程
    SKT_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)){
        SKT_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapOut() {
    SetThis(Scheduler::GetMainFiber());
    SKT_ASSERT(t_fiber != nullptr)
    if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)){
        SKT_ASSERT2(false, "swapcontext");
    }

}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    SKT_ASSERT(cur);
    try{
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception& ex){
        cur->m_state = EXCEPT;
        SKT_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << "fiber_id=" << cur->getId()
            << std::endl
            << skt::BacktraceToString();
    } catch (...){
        cur->m_state = EXCEPT;
        SKT_LOG_ERROR(g_logger) << "Fiber Except"<< "fiber_id=" << cur->getId()
                                << std::endl
                                <<skt::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr -> swapOut();//return main fiber
    SKT_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

void Fiber::CallerMainFunc() {
        Fiber::ptr cur = GetThis();
        SKT_ASSERT(cur);
        try{
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        }catch(std::exception& ex){
            cur->m_state = EXCEPT;
            SKT_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                                    << "fiber_id=" << cur->getId()
                                    << std::endl
                                    <<skt::BacktraceToString();
        } catch (...){
            cur->m_state = EXCEPT;
            SKT_LOG_ERROR(g_logger) << "Fiber Except"<< "fiber_id=" << cur->getId()
                                    << std::endl
                                    <<skt::BacktraceToString();
        }
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr -> back();//return main fiber
        SKT_ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
}

}
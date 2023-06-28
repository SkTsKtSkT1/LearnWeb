#include "fiber.h"
#include "../config/config.h"
#include "../macro/macro.h"
#include "atomic"

namespace skt{
static std::atomic<uint64_t> s_fiber_id (0);
static std::atomic<uint64_t> s_fiber_count (0);

static thread_local Fiber* t_fiber = nullptr;
static thread_local std::shared_ptr<Fiber::ptr> t_theradFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1204, "fiber stack size");

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

Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);
    if(getcontext(&m_ctx)){
        SKT_ASSERT2(false, "getcontex");
    }

    ++s_fiber_count;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize)
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

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack){
        SKT_ASSERT(m_state == TERM || m_state == INIT);

        StackAllocator::Dealloc(m_stack, m_stacksize);
    }else{
        SKT_ASSERT(!m_cb);
        SKT_ASSERT(m_state == EXEC);

        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }
}

Fiber::ptr Fiber::GetThis() {
    return nullptr;
}


void Fiber::SetThis(skt::Fiber *f) {

}

void Fiber::YieldToReady() {

}

void Fiber::YieldToHold() {

}

uint64_t Fiber::TotalFibers() {
    return 0;
}

void Fiber::reset(std::function<void()> cb) {

}

void Fiber::swapIn() {

}

void Fiber::swapOut() {

}

void Fiber::MainFunc() {

}

}
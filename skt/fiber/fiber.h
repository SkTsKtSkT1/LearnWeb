#ifndef LEARNWEB_FIBER_H
#define LEARNWEB_FIBER_H

#include "memory"
#include "ucontext.h"
#include "functional"
#include "../thread/thread.h"

namespace skt{

class Fiber : public std::enable_shared_from_this<Fiber>{
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,  //ready for exec;
        EXCEPT
    };
private:
    Fiber();
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    uint64_t getId() const {return m_id;}
    Fiber::State const getState() const {return m_state;};
    //重置协程函数，并重置状态 INIT TERM
    void reset(std::function<void()> cb);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();
public:
    //设置协程
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::ptr GetThis();
    //协程切换为后台，并且设置为Ready状态。
    static void YieldToReady();

    //协程切换为后台，并且设置为Hold状态。
    static void YieldToHold();

    //总协程数
    static uint64_t TotalFibers();

    static void MainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;
    std::function<void()> m_cb;
};

}


#endif
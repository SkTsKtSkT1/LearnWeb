#ifndef LEARNWEB_TIMER_H
#define LEARNWEB_TIMER_H
#include "memory"
#include "skt/thread/thread.h"
#include "set"

namespace skt{
class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

private:
    bool m_recurring = false;
    uint64_t  m_ms = 0; //time interval
    uint64_t  m_next = 0; //精确的执行时间
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;
private:
    struct Comparator{
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};

class TimerManager{
friend class Timer;
public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, std::weak_ptr<void> weak_cond, bool recurring = false);

protected:
    virtual void onTimerInsertAtFront() = 0;
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
};

}

#endif

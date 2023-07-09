#ifndef LEARNWEB_TIMER_H
#define LEARNWEB_TIMER_H
#include "memory"
#include "skt/thread/thread.h"
#include "vector"
#include "set"

namespace skt{
class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);
    Timer(uint64_t next);

    bool cancel();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);
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

    uint64_t getNextTimer();
    void listExpiredCb(std::vector<std::function<void()>>& cbs);

protected:
    virtual void onTimerInsertAtFront() = 0;
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);
private:
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    bool m_tickled = false;
    uint64_t  m_previouseTime = 0;
};

}

#endif

#ifndef LEARNWEB_SCHEDULER_H
#define LEARNWEB_SCHEDULER_H

#include "memory"
#include "vector"
#include "list"
#include "skt/fiber/fiber.h"
#include "skt/thread/thread.h"

namespace skt{

class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();
    const std::string& getName() const {return m_name;}

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();
    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if(need_tickle){
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle =false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock(&*begin) || need_tickle;
            }
        }
        if(need_tickle){
            tickle();
        }
    }

protected:
    virtual void tickle();
    void run();
    virtual bool stopping();
    void setThis();
    virtual void idle(); // to solve the scheduler with no work and make the thread awake
    bool hasIdleThreads() { return m_idleThreadCount > 0;}

private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread){
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb){
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }
private:
    struct FiberAndThread{
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr)
            :fiber(f), thread(thr){

        }

        FiberAndThread(Fiber::ptr *f, int thr)
            :thread(thr){
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> c, int thr)
            :cb(c) ,thread(thr){

        }

        FiberAndThread(std::function<void()>* f, int thr)
            :thread(thr){
            cb.swap(*f);
        }

        FiberAndThread()
            :thread(-1){

        }

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };
private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<FiberAndThread> m_fibers;
    Fiber::ptr m_rootFiber;
    std::string m_name;
protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idleThreadCount = {0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0; //usercaller id

};
}

#endif
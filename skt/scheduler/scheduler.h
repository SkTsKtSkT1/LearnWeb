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
    void Start();
    void Stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){

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
    std::string m_name;

};
}

#endif
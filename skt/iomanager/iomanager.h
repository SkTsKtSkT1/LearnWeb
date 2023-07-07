#ifndef LEARNWEB_IOMANAGER_H
#define LEARNWEB_IOMANAGER_H

#include "skt/scheduler/scheduler.h"

namespace skt{
class IOManager : public Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event{
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4,
    };
private:
    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* scheduler = nullptr; //exec in which scheduler;
            Fiber::ptr fiber;     //事件的协程
            std::function<void()> cb;//事件的回调函数
        };

        int fd = 0;                 //事件关联的句柄

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerEvent(Event event);

        EventContext read;      // 读事件
        EventContext write;     //写事件
        Event events = NONE; //已经注册的事件
        MutexType mutex;
    };
public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    //success 0, error -1
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void contextResize(size_t size);

private:
    int m_epfd = 0;
    int m_tickleFds[2];

    std::atomic<size_t> m_pendingEventCount = {0};
    std::vector<FdContext*> m_fdContexts;
    RWMutexType m_mutex;
};
}

#endif

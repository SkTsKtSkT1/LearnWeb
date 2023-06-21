#include "thread.h"
#include "../log/log.h"
#include "../util/util.h"

namespace  skt{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

Thread* Thread::GetThis(){
    return t_thread;
}
const std::string& Thread::GetName(){
    return t_thread_name;
}

void Thread::setName(const std::string &name) {
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt) {
        SKT_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
}



Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);
    }
}

void Thread::join(){
    if(m_thread){
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SKT_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*) arg;
    t_thread = thread;
    thread->m_id = skt::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);
    cb();
    return nullptr;
}




}

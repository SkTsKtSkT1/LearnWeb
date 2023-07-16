#ifndef LEARNWEB_MUTEX_H
#define LEARNWEB_MUTEX_H

#include "thread"
#include "functional"
#include "memory"
#include "string"
#include "pthread.h"
#include "semaphore.h"
#include "stdint.h"
#include "atomic"
#include "stdexcept"
#include "skt/noncopyable/noncopyable.h"
namespace skt{

class Semaphore : Noncopyable{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
private: //disable
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    sem_t m_semaphore;
};

template<class T>
struct ScopedLockImpl{
public:
    ScopedLockImpl(T& mutex)
            :m_mutex(mutex){
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

//if 读写差不多，不分读写锁
class Mutex : Noncopyable{
public:
    typedef ScopedLockImpl<Mutex> Lock;

    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

//读写锁
template<class T>
struct ReadScopedLockImpl{
public:
    ReadScopedLockImpl(T& mutex)
            :m_mutex(mutex){
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class NullMutex : Noncopyable{
public:
    typedef ScopedLockImpl<NullMutex> Lock;
    NullMutex(){};
    ~NullMutex(){};
    void lock(){};
    void unlock(){};
};

class Spinlock : Noncopyable{
public:
    typedef ScopedLockImpl<Spinlock> Lock;

    Spinlock(){
        pthread_spin_init(&m_mutex, 0);
    }

    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

template<class T>
struct WriteScopedLockImpl{
public:
    WriteScopedLockImpl(T& mutex)
            :m_mutex(mutex){
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl(){
        unlock();
    }

    void lock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class RWMutex : Noncopyable{
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;

    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};

class NullRWMutex : Noncopyable{
public:
    typedef ReadScopedLockImpl<NullRWMutex> ReadLock;
    typedef WriteScopedLockImpl<NullRWMutex> WriteLock;
    NullRWMutex(){};
    ~NullRWMutex(){};
    void unlock(){};
    void rdlock(){};
    void wrlock(){};
};

class CASLock : Noncopyable{
public:
    typedef ScopedLockImpl<CASLock> Lock;
    CASLock(){

    }

    ~CASLock(){

    }

    void lock(){
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock(){
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_relaxed);
    }
private:
    volatile std::atomic_flag m_mutex;
};


}

#endif

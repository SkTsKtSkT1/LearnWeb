#include "fd_manager.h"
#include "sys/stat.h"
#include "skt/hook/hook.h"

namespace skt{

FdCtx::FdCtx(int fd)
    :m_isInit(false)
    ,m_isSocket(false)
    ,m_sysNonblock(false)
    ,m_userNonblock(false)
    ,m_isClosed(false)
    ,m_fd(fd)
    ,m_recvTimeout(-1)
    ,m_sendTimeout(-1){
    init();
}

FdCtx::~FdCtx() {

}

bool FdCtx::init() {
    if(m_isInit){
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;
    struct stat fd_stat;
    if(-1 == fstat(m_fd, &fd_stat)){
        m_isInit = false;
        m_isSocket = false;
    }else{
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if(m_isSocket){
        int flags = fcntl_f(m_fd, F_GETFL, 0); // 23.7.27, bug when test_socket, and the reason is here, use fcntl(error）
        //因为调用了get方法， 但是没有找到导致return ,没有释放🔓 m_mutex, 导致死锁
        if(!(flags & O_NONBLOCK)){
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    }else{
        m_sysNonblock = false;
    }

    m_userNonblock = false;
    m_isClosed = false;
    return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t v) {
    if(type == SO_RCVTIMEO){
        m_recvTimeout = v;
    }else{
        m_sendTimeout = v;
    }
}

uint64_t FdCtx::getTimeout(int type) {
    if(type == SO_RCVTIMEO){
        return m_recvTimeout;
    }else{
        return m_sendTimeout;
    }
}

FdManager::FdManager() {
    m_datas.resize(64);
}

FdCtx::ptr FdManager::get(int fd, bool auto_create) {
    if(fd == -1){
        return nullptr;
    }
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_datas.size() <= fd){
        if(auto_create == false){
            return nullptr;
        }
    }else{
        if(m_datas[fd] || !auto_create){
            return m_datas[fd];
        }
    }
    lock.unlock();

    //if there do not exist the fd;
    RWMutexType::WriteLock lock2(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(fd >= (int)m_datas.size()) {
        m_datas.resize(fd * 1.5);
    }
    m_datas[fd] = ctx;
    return ctx;
}

void FdManager::del(int fd) {
    RWMutexType::WriteLock lock(m_mutex);
    if((int)m_datas.size() <= fd){
        return;
    }
    m_datas[fd].reset();

}





}
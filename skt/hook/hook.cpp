#include "hook.h"
#include "dlfcn.h"

#include "skt/fiber/fiber.h"
#include "skt/timer/timer.h"
#include "skt/iomanager/iomanager.h"
#include "skt/log/log.h"

namespace skt{

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");
static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep)        \
    XX(usleep)

void hook_init(){
    static bool is_inited = false;
    if(is_inited){
        return;
    }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

struct _HookIniter{
    _HookIniter(){
         hook_init();
    }
};

static _HookIniter s_hook_initer;

bool is_hook_enable(){
    return t_hook_enable;
}


void set_hook_enable(bool flag){
    t_hook_enable = flag;
}

extern "C"{
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX)
#undef XX

unsigned int sleep(unsigned int seconds){
    if(!skt::t_hook_enable){
        return sleep_f(seconds);
    }
    skt::Fiber::ptr fiber = skt::Fiber::GetThis();
    SKT_LOG_DEBUG(g_logger) << "sleep() and fiber id=" << fiber->getId();
    skt::IOManager* iom = skt::IOManager::GetThis();
    //iom->addTimer(seconds * 1000, std::bind(&skt::IOManager::schedule, iom, fiber));
    iom->addTimer(seconds * 1000, [iom, fiber](){
        iom->schedule(fiber);
    });
    skt::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec){
    if(!skt::t_hook_enable){
        return usleep_f(usec);
    }
    skt::Fiber::ptr fiber = skt::Fiber::GetThis();
    skt::IOManager* iom = skt::IOManager::GetThis();
    //iom->addTimer(usec / 1000, std::bind(&skt::IOManager::schedule, iom, fiber));
    iom->addTimer(usec / 1000, [iom, fiber](){
        iom->schedule(fiber);
    });
    skt::Fiber::YieldToHold();
    return 0;
}

}

}
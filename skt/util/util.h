#ifndef LEARNWEB_UTIL_H
#define LEARNWEB_UTIL_H

#include "pthread.h"
#include "sys/types.h"
#include "sys/syscall.h"
#include "unistd.h"

namespace skt{

pid_t GetThreadId();
u_int32_t GetFiberId();

}

#endif //LEARNWEB_UTIL_H
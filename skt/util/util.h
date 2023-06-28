#ifndef LEARNWEB_UTIL_H
#define LEARNWEB_UTIL_H

#include "pthread.h"
#include "sys/types.h"
#include "sys/syscall.h"
#include "unistd.h"
#include "vector"
#include "string"

namespace skt{

pid_t GetThreadId();
u_int32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);//全局方法
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");
}

#endif //LEARNWEB_UTIL_H
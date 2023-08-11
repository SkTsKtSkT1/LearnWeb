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

//时间ms
uint64_t GetCurrentMs();
uint64_t GetCurrentUs();

std::string Time2Str(time_t ts, const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

class FSUtil{
public:
    static void ListAllFile(std::vector<std::string>& files, const std::string& path, const std::string& subfix);
    static bool Mkdir(const std::string& dirname);
    static bool IsRunningPidfile(const std::string& pidfile);
};
}

#endif //LEARNWEB_UTIL_H
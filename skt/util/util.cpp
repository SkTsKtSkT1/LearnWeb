#include "util.h"
#include "execinfo.h"
#include "skt/log/log.h"
#include "skt/fiber/fiber.h"
#include "sys/time.h"
namespace skt{

skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

pid_t GetThreadId(){
    return syscall(SYS_gettid);
}

u_int32_t GetFiberId(){
    return skt::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**) malloc((sizeof(void*) * size)); //不在栈分配太大的
    size_t s= ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if(strings == nullptr){
        SKT_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return ;
    }
    for(size_t i = skip; i < s; ++i){
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); ++i){
        ss << prefix << bt[i] << std::endl;
    }
    return ss.str();
}

uint64_t GetCurrentMs(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUs(){
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}



}

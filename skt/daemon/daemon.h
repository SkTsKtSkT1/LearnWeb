#ifndef LEARNWEB_DAEMON_H
#define LEARNWEB_DAEMON_H

#include <functional>
#include <unistd.h>
#include "skt/singleton/singleton.h"

namespace skt{


struct ProcessInfo{
    pid_t parent_id;
    pid_t main_id;
    uint64_t parent_start_time = 0;
    uint64_t main_start_time = 0;
    uint32_t restart_count = 0;

    std::string toString() const;
};

typedef skt::Singleton<ProcessInfo> ProcessInfoMgr;

/**
 * @brief 启动程序可以选择用守护进程的方式
 * @param[in] argc 参数个数
 * @param[in] argv 参数值数组
 * @param[in] main_cb 启动函数
 * @param[in] is_daemon 是否守护进程的方式
 * @return 返回程序的执行结果
 */
int start_daemon(int argc, char** argcv, std::function<int(int argc, char** argv)> main_cb, bool is_daemon);

}
#endif

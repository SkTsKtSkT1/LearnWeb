#include "skt/daemon/daemon.h"
#include "skt/config/config.h"
#include "skt/log/log.h"
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

namespace skt{

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

static skt::ConfigVar<uint32_t>::ptr g_daemon_restart_interval
        = skt::Config::Lookup("daemon.restart_interval", (uint32_t)5, "daemon restart interval");

std::string ProcessInfo::toString() const {
    std::stringstream ss;
    ss << "[ProcessInfo parent_id=" << parent_id
       << " main_id=" << main_id
       << " parent_start_time=" << skt::Time2Str(parent_start_time)
       << " main_start_time=" << skt::Time2Str(main_start_time)
       << " restart_count=" << restart_count << "]";
    return ss.str();
}

static int real_start(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb){
    ProcessInfoMgr::GetInstance()->parent_id = getpid();
    ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
    return main_cb(argc, argv);
}

static int real_daemon(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb){
    daemon(1, 0);
    ProcessInfoMgr::GetInstance()->parent_id = getpid();
    ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
    while(true){
        pid_t pid = fork();
        if(pid == 0){
            //子进程返回
            ProcessInfoMgr::GetInstance()->main_id = getpid();
            ProcessInfoMgr::GetInstance()->main_start_time = time(0);
            SKT_LOG_INFO(g_logger) << "process start pid=" << pid;
            return real_start(argc, argv, main_cb);
        }else if(pid < 0){
            SKT_LOG_ERROR(g_logger) << "fork fail return=" << pid
                << " errno=" << errno << " errstr=" << strerror(errno);
            return -1;
        }else{
            //父进程返回
            int status = 0;
            waitpid(pid, &status, 0);
            if(status){
                if(status == 9) {
                    SKT_LOG_INFO(g_logger) << "killed";
                    break;
                } else {
                    SKT_LOG_ERROR(g_logger) << "child crash pid=" << pid
                                              << " status=" << status;
                }
            }else {
                SKT_LOG_INFO(g_logger) << "child finished pid=" << pid;
                break;
            }
            ProcessInfoMgr::GetInstance()->restart_count += 1;
            sleep(g_daemon_restart_interval->getValue());
        }
    }
    return 0;
}

int start_daemon(int argc, char** argv, std::function<int(int argc, char** argv)> main_cb, bool is_daemon){
    if(!is_daemon){
        ProcessInfoMgr::GetInstance()->parent_id = getpid();
        ProcessInfoMgr::GetInstance()->parent_start_time = time(0);
        return real_start(argc, argv, main_cb);
    }
    return real_daemon(argc, argv, main_cb);
}
}
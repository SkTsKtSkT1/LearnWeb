#include "skt/daemon/daemon.h"
#include "skt/iomanager/iomanager.h"
#include "skt/log/log.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();
skt::Timer::ptr timer;
int server_main(int argc, char** argv){
    SKT_LOG_INFO(g_logger) << skt::ProcessInfoMgr::GetInstance()->toString();
    skt::IOManager iom(1);

    timer = iom.addTimer(1000, [](){
        SKT_LOG_INFO(g_logger) << "onTimer";
        static int count = 0;
        if(++count > 10){
            timer->cancel();
        }
    }, true);
    return 0;
}

int main(int argc, char** argv){
    return skt::start_daemon(argc, argv, server_main, argc != 1);
}
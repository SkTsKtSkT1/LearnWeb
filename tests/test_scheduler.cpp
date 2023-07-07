#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_fiber(){
    static int s_count = 5;
    SKT_LOG_INFO(g_logger) << "test in fiber, s_count=" << s_count;
    sleep(1);
    if(--s_count >= 0){
        skt::Scheduler::GetThis()->schedule(&test_fiber, skt::GetThreadId());
    }
}

int main(int argc, char** argv){
    SKT_LOG_INFO(g_logger) << "main";
    skt::Scheduler sc(3, false, "test");
    sc.start();
    SKT_LOG_INFO(g_logger) << "scheduler";
    sc.schedule(&test_fiber);
    sc.stop();
    SKT_LOG_INFO(g_logger) << "over";
    return 0;
}
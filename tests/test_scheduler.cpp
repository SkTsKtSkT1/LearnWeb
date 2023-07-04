#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_fiber(){
    SKT_LOG_INFO(g_logger) << "test in fiber";
}

int main(int argc, char** argv){
    SKT_LOG_INFO(g_logger) << "main";
    skt::Scheduler sc;
    sc.schedule(&test_fiber);
    sc.start();
    sc.stop();
    SKT_LOG_INFO(g_logger) << "over";
    return 0;
}
#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_sleep(){
    skt::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        SKT_LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([](){
        sleep(3);
        SKT_LOG_INFO(g_logger) << "sleep 3";
    });
    SKT_LOG_INFO(g_logger) << "test_sleep";
}

int main(int argc, char** argv){
    test_sleep();
    return 0;
}
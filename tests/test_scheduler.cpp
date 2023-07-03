#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

int main(int argc, char** argv){
    skt::Scheduler sc;
    sc.start();
    sc.stop();
    return 0;
}
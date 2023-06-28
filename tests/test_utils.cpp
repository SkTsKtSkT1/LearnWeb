#include "skt/skt.h"
#include "assert.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_assert(){
    SKT_LOG_INFO(g_logger) << skt::BacktraceToString(10);
    //SKT_ASSERT(false);
    SKT_ASSERT2(0 == 1, "abcdef xx");
}

int main(int argc, char** argv){
    test_assert();
    return 0;
}


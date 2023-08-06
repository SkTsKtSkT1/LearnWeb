#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void YieldHold(skt::Fiber::ptr cur) {
    SKT_ASSERT(cur->getState() == skt::Fiber::EXEC);
    cur->back();
}

void run_in_fiber(){
    SKT_LOG_INFO(g_logger) << "run_in_fiber begin";
    //skt::Fiber::GetThis()->YieldToHold();
    YieldHold(skt::Fiber::GetThis());
    SKT_LOG_INFO(g_logger) << "run_in_fiber end";
    //skt::Fiber::YieldToHold();
    YieldHold(skt::Fiber::GetThis());
}

void test_fiber(){
    SKT_LOG_INFO(g_logger) << "main begin -1";
    {
        skt::Fiber::GetThis();
        SKT_LOG_INFO(g_logger) << "main begin";
        skt::Fiber::ptr fiber(new skt::Fiber(run_in_fiber, 0, true));
        fiber->call();
        SKT_LOG_INFO(g_logger) << "main after swapIn";
        fiber->call();
        SKT_LOG_INFO(g_logger) << "main after end";
        fiber->call();
    }
    SKT_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char** argv){
    skt::Thread::SetName("main");
    std::vector<skt::Thread::ptr> thrs;
    for(int i = 0; i < 3; ++i){
        thrs.push_back(skt::Thread::ptr(
                new skt::Thread(test_fiber, "name_" + std::to_string(i))));
    }
    for(auto i :thrs){
        i->join();
    }
    return 0;
}
#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void func1(){
    SKT_LOG_INFO(g_logger) << "name: " << skt::Thread::GetName()
                            << " this.name: " << skt::Thread::GetThis()->getName()
                            << " id: " << skt::GetThreadId()
                            << " this.id: " << skt::Thread::GetThis()->getId();

}

void func2(){

}

int main(){
    SKT_LOG_INFO(g_logger) << "thread test begin";
    std::vector<skt::Thread::ptr> thrs;
    for(int i = 0; i < 5; ++i){
        skt::Thread::ptr thr(new skt::Thread(&func1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i <5 ; ++i){
        thrs[i]->join();
    }

    SKT_LOG_INFO(g_logger) << "thread test end";
    return 0;
}
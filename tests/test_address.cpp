#include "skt/skt.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test(){
    std::vector<skt::Address::ptr> addrs;

    SKT_LOG_INFO(g_logger) << "begin";
    bool v = skt::Address::Lookup(addrs, "www.sylar.top");
    if(!v){
        SKT_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }
    for(size_t i = 0; i< addrs.size(); ++i){
        SKT_LOG_INFO(g_logger) << i << "-" << addrs[i]->toString();
    }
}

void test_iface(){
    
}

int main(int argc, char** argv){
    test();
    return 0;
}
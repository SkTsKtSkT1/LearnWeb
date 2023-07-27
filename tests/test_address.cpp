#include "skt/skt.h"
#include "map"
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
    std::multimap<std::string, std::pair<skt::Address::ptr, uint32_t>> results;
    bool v = skt::Address::GetInterfaceAddresses(results);
    if(!v){
        SKT_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
        return;
    }

    for(auto& i : results){
        SKT_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - " << i.second.second;
    }
}

void test_ipv4(){
    //auto addr = skt::IPAddress::Create("www.sylar.top");
    auto addr = skt::IPAddress::Create("127.0.0.8");
    if(addr){
        SKT_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char** argv){
    test();
    //test_iface();
    //test_ipv4();
    return 0;
}
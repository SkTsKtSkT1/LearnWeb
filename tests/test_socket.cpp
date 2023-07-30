#include "skt/skt.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_socket() {
//    std::vector<skt::Address::ptr> addrs;
//    skt::Address::Lookup(addrs, "www.baidu.com", AF_INET);
//    skt::IPAddress::ptr addr;
//    for(auto i : addrs){
//        SKT_LOG_INFO(g_logger) << i->toString();
//        addr = std::dynamic_pointer_cast<skt::IPAddress>(i);
//        if(addr){
//            break;
//        }
//    }
    skt::IPAddress::ptr addr = skt::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        SKT_LOG_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        SKT_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    skt::Socket::ptr sock = skt::Socket::CreateTCP(addr);
    addr->setPort(80);
    SKT_LOG_DEBUG(g_logger) << "addr= " << addr->toString();
    if (!sock->connect(addr)) {
        SKT_LOG_ERROR(g_logger) << "connect " << addr->toString() << " fail";
    } else {
        SKT_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0) {
        SKT_LOG_INFO(g_logger) << "send fail rt=" << rt;
        return;
    }

    std::string buffs;
    buffs.resize(4096);

    rt = sock->recv(&buffs[0], buffs.size());
    if (rt <= 0) {
        SKT_LOG_INFO(g_logger) << "recv fail rt=" << rt;
        return;
    }
    buffs.resize(rt);
    SKT_LOG_INFO(g_logger) << buffs;
}

int main(int argc, char** argv){
    skt::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}
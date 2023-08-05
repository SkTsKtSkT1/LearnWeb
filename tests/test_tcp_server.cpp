#include "skt/tcp_server/tcp_server.h"
#include "skt/iomanager/iomanager.h"
#include "skt/log/log.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void run(){
    auto addr = skt::Address::LookupAny("0.0.0.0:1234");
    //auto addr2 = skt::UnixAddress::ptr(new skt::UnixAddress("/tmp/unix_addr"));
    //SKT_LOG_INFO(g_logger) << *addr << " - " << *addr2;

    std::vector<skt::Address::ptr> addrs;
    addrs.push_back(addr);
    //addrs.push_back(addr2);

    skt::TcpServer::ptr tcp_server(new skt::TcpServer);
    std::vector<skt::Address::ptr> fails;
    while(!tcp_server->bind(addrs, fails)){
        sleep(2);
    }
    tcp_server->start();
}

int main(int argc, char** argv){
    skt::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
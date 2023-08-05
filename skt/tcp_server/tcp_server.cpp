#include "tcp_server.h"
#include "skt/config/config.h"

namespace skt{

static skt::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
        skt::Config::Lookup("tcp_server.read_timeout", (uint64_t)(60 * 1000 * 2),
                              "tcp server read timeout");

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

TcpServer::TcpServer(skt::IOManager *worker, skt::IOManager* accept_worker)
    :m_worker(worker)
    ,m_acceptWorker(accept_worker)
    ,m_recvTimeout(g_tcp_server_read_timeout->getValue())
    ,m_name("skt/1.0.0")
    ,m_isStop(true){

}

TcpServer::~TcpServer(){
    for(auto& i : m_socks){
        i->close();
    }
    m_socks.clear();
}

bool TcpServer::bind(skt::Address::ptr addr) {
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr>& fails) {
    for(auto& addr : addrs){
        Socket::ptr sock = Socket::CreateTCP(addr);
        if(!sock->bind(addr)){
            SKT_LOG_ERROR(g_logger) << "bind fail errno:" << errno
                << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        if(!sock->listen()){
            SKT_LOG_ERROR(g_logger) << "listen fail errno=" << errno
                << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
    }
    //有失败就失败了~
    if(!fails.empty()){
        m_socks.clear();
        return false;
    }

    for(auto& sock : m_socks){
        SKT_LOG_INFO(g_logger) << "name=" << m_name
            << " server bind success: " << *sock;
    }
    return true;
}

bool TcpServer::start() {
    if(!m_isStop){
        return true;
    }
    m_isStop = false;
    for(auto& sock : m_socks){
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept, shared_from_this(), sock));
    }
    return true;
}

void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self](){
        for(auto& sock : m_socks){
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}

void TcpServer::handleClient(Socket::ptr client) {
    SKT_LOG_INFO(g_logger) << "handleClient: " << *client;
}

void TcpServer::startAccept(Socket::ptr sock) {
    while(!m_isStop){
        Socket::ptr client = sock->accept();
        if(client){
            client->setRecvTimeout(m_recvTimeout);
            m_worker->schedule(std::bind(&TcpServer::handleClient, shared_from_this(), client));
        }else{
            SKT_LOG_ERROR(g_logger) << "accept errno=" << errno
                << " errstr=" << strerror(errno);
        }
    }
}

}
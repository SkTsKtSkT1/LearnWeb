#ifndef LEARNWEB_TCP_SERVER_H
#define LEARNWEB_TCP_SERVER_H

#include "memory"
#include "functional"
#include "skt/iomanager/iomanager.h"
#include "skt/noncopyable/noncopyable.h"
#include "skt/socket/socket.h"

namespace skt{ 

class TcpServer : public std::enable_shared_from_this<TcpServer>, Noncopyable{
public:
    typedef std::shared_ptr<TcpServer> ptr;

    TcpServer(skt::IOManager* worker = skt::IOManager::GetThis(),
              skt::IOManager* accept_worker = skt::IOManager::GetThis());
    virtual ~TcpServer();

    virtual bool bind(skt::Address::ptr addr);
    virtual bool bind(const std::vector<Address::ptr>& addrs, std::vector<Address::ptr>& fails);
    virtual bool start();
    virtual void stop();

    uint64_t getRecvTimeout() const { return m_recvTimeout;}
    std::string getName() const { return m_name;}

    void setRecvTimeout(uint64_t v) { m_recvTimeout = v;}
    void setName(const std::string& v) { m_name = v;}

    bool isStop() { return m_isStop;}

protected:
    virtual void handleClient(Socket::ptr client);
    virtual void startAccept(Socket::ptr sock);
private:
    std::vector<Socket::ptr> m_socks;

    IOManager* m_worker;
    IOManager* m_acceptWorker;
    uint64_t m_recvTimeout;

    std::string m_name;

    bool m_isStop;


};

}
#endif

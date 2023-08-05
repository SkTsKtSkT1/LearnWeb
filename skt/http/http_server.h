#ifndef LEARNWEB_HTTP_SERVER_H
#define LEARNWEB_HTTP_SERVER_H

#include "skt/tcp_server/tcp_server.h"
#include "skt/http/http_session.h"
#include "skt/http/http_parser.h"

namespace skt{
namespace http{

class HttpServer : public TcpServer{
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false,
               skt::IOManager* worker = skt::IOManager::GetThis(),
               skt::IOManager* accept_worker = skt::IOManager::GetThis());
protected:
    virtual void handleClient(Socket::ptr client) override;

protected:
    bool m_isKeepalive;
};

}
}

#endif

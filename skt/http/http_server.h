#ifndef LEARNWEB_HTTP_SERVER_H
#define LEARNWEB_HTTP_SERVER_H

#include "skt/tcp_server/tcp_server.h"
#include "skt/http/http_session.h"
#include "skt/http/http_parser.h"
#include "skt/http/servlet.h"


namespace skt{
namespace http{

class HttpServer : public TcpServer{
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false,
               skt::IOManager* worker = skt::IOManager::GetThis(),
               skt::IOManager* accept_worker = skt::IOManager::GetThis());

    ServletDispatch::ptr getServletDispatch() const {return m_dispatch;}
    void setServletDispatch(ServletDispatch::ptr v) {m_dispatch = v;}
protected:
    virtual void handleClient(Socket::ptr client) override;

protected:
    bool m_isKeepalive;
    ServletDispatch::ptr m_dispatch;
};

}
}

#endif

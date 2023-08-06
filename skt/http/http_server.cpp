#include "http_server.h"
#include "skt/log/log.h"

namespace skt{
namespace http{
static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive, skt::IOManager *worker, skt::IOManager *accept_worker)
    :TcpServer(worker, accept_worker)
    ,m_isKeepalive(keepalive){
    m_dispatch.reset(new ServletDispatch);
}

void HttpServer::handleClient(Socket::ptr client) {
    skt::http::HttpSession::ptr session(new skt::http::HttpSession(client));
    do{
        auto req = session->recvRequest();
        if(!req){
            SKT_LOG_WARN(g_logger) << "recv http request fail, errno="
                << errno << " errstr=" << strerror(errno)
                << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
            break;
        }

        HttpResponse::ptr rsp(new skt::http::HttpResponse(req->getVersion(),
            req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
//        rsp->setBody("hello skt");
//        SKT_LOG_INFO(g_logger) << "request:" << std::endl << *req;
//        SKT_LOG_INFO(g_logger) << "response:" << std::endl << *rsp;
        session->sendResponse(rsp);
        if(!m_isKeepalive || req->isClose()){
            break;
        }
    }while(true);
    session->close();
}
}
}
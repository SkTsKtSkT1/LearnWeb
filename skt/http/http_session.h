#ifndef LEARNWEB_HTTP_SESSION_H
#define LEARNWEB_HTTP_SESSION_H

#include "http.h"
#include "skt/streams/socket_stream.h"

namespace skt{

namespace http{
//Server.accept, socket->session
class HttpSession : public SocketStream {
public:
    typedef std::shared_ptr<HttpSession> ptr;
    HttpSession(Socket::ptr sock, bool owner = true);

    HttpRequest::ptr recvRequest();
    int sendResponse(HttpResponse::ptr rsp);
};

}
}

#endif
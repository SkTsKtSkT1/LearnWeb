#ifndef LEARNWEB_HTTP_CONNECTION_H
#define LEARNWEB_HTTP_CONNECTION_H

#include "http.h"
#include "skt/streams/socket_stream.h"

namespace skt{

namespace http{
//Server.accept, socket->session
class HttpConnection : public SocketStream {
public:
    typedef std::shared_ptr<HttpConnection> ptr;
    HttpConnection(Socket::ptr sock, bool owner = true);

    HttpResponse::ptr recvResponse();
    int sendRequest(HttpRequest::ptr rsp);
};

}
}
#endif
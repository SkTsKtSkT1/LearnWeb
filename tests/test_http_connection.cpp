#include <iostream>
#include "skt/http/http_connection.h"
#include "skt/http/http_parser.h"
#include "skt/log/log.h"
#include "skt/iomanager/iomanager.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void run(){
    skt::Address::ptr addr = skt::Address::LookupAnyIPAddress("www.sylar.top:80");
    if(!addr){
        SKT_LOG_INFO(g_logger) << "get addr error";
        return ;
    }
    skt::Socket::ptr sock = skt::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if(!rt){
        SKT_LOG_INFO(g_logger) << "connect" << *addr << " failed";
        return;
    }

    skt::http::HttpConnection::ptr conn(new skt::http::HttpConnection(sock));
    skt::http::HttpRequest::ptr req(new skt::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setPath("/blog/");
    SKT_LOG_INFO(g_logger) << "req:" << '\n'
        <<  *req;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    if(!rsp){
        SKT_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    SKT_LOG_INFO(g_logger) << "rsp:" << '\n'
        << *rsp;
}

int main(int argc, char** argv){
    skt::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
#include <iostream>
#include "skt/http/http_connection.h"
#include "skt/http/http_parser.h"
#include "skt/log/log.h"
#include "skt/iomanager/iomanager.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_pool(){
    skt::http::HttpConnectionPool::ptr pool(new skt::http::HttpConnectionPool("www.sylar.top", "", 80, 10, 1000 * 30, 5));
    skt::IOManager::GetThis()->addTimer(1000, [pool](){
       auto r= pool->doGet("/", 300);
       SKT_LOG_INFO(g_logger) << r->toString();
    },true);
}

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

    SKT_LOG_INFO(g_logger) << "===========================";

    auto r = skt::http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300);
    SKT_LOG_INFO(g_logger) << "result=" << r->result
        << " error=" << r->error
        << " rsp=" << (r->response ? r->response->toString() : "");
}

int main(int argc, char** argv){
    skt::IOManager iom(2);
    //iom.schedule(run);
    iom.schedule(test_pool);
    return 0;
}
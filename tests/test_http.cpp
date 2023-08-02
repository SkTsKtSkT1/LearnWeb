#include "skt/http/http.h"
#include "skt/log/log.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();
void test_request(){
    skt::http::HttpRequest::ptr req(new skt::http::HttpRequest);
    req->setHeader("host", "www.sylar.com");
    req->setBody("hello sylar");

    req->dump(std::cout) << std::endl;
}

void test_response(){
    skt::http::HttpResponse::ptr rsp(new skt::http::HttpResponse);
    rsp->setHeader("X-X", "sylar");
    rsp->setBody("hello sylar");
    rsp->setStatus((skt::http::HttpStatus)400);
    rsp->setClose(false);

    rsp->dump(std::cout) << std::endl;
}

int main(int argc, char** argv){
    test_request();
    test_response();
    return 0;
}
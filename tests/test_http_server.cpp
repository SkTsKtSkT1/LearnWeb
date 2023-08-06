#include "skt/http/http_server.h"
#include "skt/log/log.h"
#include "skt/http/servlet.h"
#include "skt/http/http_session.h"
#include <functional>


static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void run(){
    skt::http::HttpServer::ptr server(new skt::http::HttpServer);
    skt::Address::ptr addr = skt::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while(!server->bind(addr)){
        sleep(2);
    }
    auto sd = server->getServletDispatch();

    sd->addServlet("/skt/xx", [](skt::http::HttpRequest::ptr req
            ,skt::http::HttpResponse::ptr rsp
            ,skt::http::HttpSession::ptr session) {
        rsp->setBody(req->toString());
        return 0;
    });

    sd->addGlobServlet("/skt/*", [](skt::http::HttpRequest::ptr req, skt::http::HttpResponse::ptr rsp, skt::http::HttpSession::ptr session){
        rsp->setBody("Glob:\r\n" + req->toString());
        return 0;
    });
    server->start();
}

int main(int argc, char** argv){
    skt::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
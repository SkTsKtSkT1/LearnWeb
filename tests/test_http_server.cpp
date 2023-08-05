#include "skt/http/http_server.h"
#include "skt/log/log.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void run(){
    skt::http::HttpServer::ptr server(new skt::http::HttpServer);
    skt::Address::ptr addr = skt::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while(!server->bind(addr)){
        sleep(2);
    }
    server->start();
}

int main(int argc, char** argv){
    skt::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
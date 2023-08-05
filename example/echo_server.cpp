#include "skt/tcp_server/tcp_server.h"
#include "skt/log/log.h"
#include "skt/iomanager/iomanager.h"
#include "skt/socket/socket.h"
#include "skt/bytearray/bytearray.h"

static skt::Logger::ptr g_logger = SKT_LOG_ROOT();

class EchoServer : public skt::TcpServer{
public:
    EchoServer(int type);
    void handleClient(skt::Socket::ptr client) override;
private:
    int m_type = 0;
};


EchoServer::EchoServer(int type)
    :m_type(type){

}

void EchoServer::handleClient(skt::Socket::ptr client) {
    SKT_LOG_INFO(g_logger) << "handleClient" << *client;
    skt::ByteArray::ptr ba(new skt::ByteArray);
    while(true){
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);
        int rt = client->recv(&iovs[0], iovs.size());
        if(rt == 0){
            SKT_LOG_INFO(g_logger) << "client close:" << *client;
            break;
        }else if(rt < 0){
            SKT_LOG_INFO(g_logger) << "client error rt=" << rt
                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt); //不会修改position and size,
        ba->setPosition(0);
        //SKT_LOG_INFO(g_logger) << "recv rt=" << rt << " data=" << std::string((char*)(iovs[0].iov_base), rt);
        if(m_type == 1){
            std::cout << ba->toString() << std::endl;
        }else{
            std::cout << ba->toHexString() << std::endl;
        }
        std::cout.flush();
    }
}


int type = 1;

void run(){
    SKT_LOG_INFO(g_logger) << "server type=" << type;
    EchoServer::ptr es(new EchoServer(type));
    auto addr = skt::Address::LookupAny("0.0.0.0:8020");
    while(!es->bind(addr)){
        sleep(2);
    }
    es->start();
}

int main(int argc, char** argv){
    if(argc < 2){
        SKT_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
    }
    if(!strcmp(argv[1], "-b")){
        type = 2;
    }

    skt::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
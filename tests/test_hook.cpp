#include "skt/skt.h"
#include <arpa/inet.h>
#include "sys/socket.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

void test_sleep(){
    skt::IOManager iom(1);
    iom.schedule([](){
        sleep(2);
        SKT_LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([](){
        sleep(3);
        SKT_LOG_INFO(g_logger) << "sleep 3";
    });
    SKT_LOG_INFO(g_logger) << "test_sleep";
}

void test_socket(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "182.61.200.6", &addr.sin_addr.s_addr);

    SKT_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    SKT_LOG_INFO(g_logger) << "connect rt=" << rt << " errno=" << errno;

    if(rt){
        close(sock);
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    SKT_LOG_INFO(g_logger) << "send rt=" << rt << " errno=" << errno;

    if(rt <= 0){
        close(sock);
        return ;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    SKT_LOG_INFO(g_logger) << "recv rt=" << rt << " errno=" << errno;
    if(rt <= 0){
        close(sock);
        return ;
    }

    buff.resize(rt);
    SKT_LOG_INFO(g_logger) << buff;
    close(sock);
}

int main(int argc, char** argv){
    //test_sleep();
    //test_socket();
    skt::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}
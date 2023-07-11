#include <arpa/inet.h>
#include "skt/iomanager/iomanager.h"
#include "skt/skt.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "fcntl.h"
#include "unistd.h"

skt::Logger::ptr g_logger = SKT_LOG_ROOT();

int sock = 0;

void test_fiber(){
    SKT_LOG_INFO(g_logger) << "test_fiber sock=" << sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "182.61.200.6", &addr.sin_addr.s_addr);

    if(!connect(sock, (const sockaddr*)&addr, sizeof(addr))){
    }else if(errno == EINPROGRESS){
        SKT_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);
        skt::IOManager::GetThis()->addEvent(sock, skt::IOManager::READ, [](){
            SKT_LOG_INFO(g_logger) << "read callback";
        });
        skt::IOManager::GetThis()->addEvent(sock, skt::IOManager::WRITE, [](){
            SKT_LOG_INFO(g_logger) << "write callback";
            //close(sock);
            skt::IOManager::GetThis()->cancelEvent(sock, skt::IOManager::READ);
            close(sock);
        });
    }else{
        SKT_LOG_INFO(g_logger) << "else" << errno << " " << strerror(errno);
    }
}

void test1(){
    skt::IOManager iom(2, false);
    iom.schedule(&test_fiber);
}

skt::Timer::ptr s_timer;
void test_timer(){
    skt::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        SKT_LOG_INFO(g_logger) << "hello timer i=" << i;
        if(++i == 3){
            s_timer->reset(2000, true);
            //s_timer->cancel();
        }
        }, true);
}

int main(int argc, char** argv){
    test_timer();
    return 0;
}
//
// Created by 25467 on 2023/6/11.
//

#include "iostream"
#include "../skt/log/log.h"
#include "../skt/util/util.h"

int main(int argc, char** argv){
    skt::Logger::ptr logger(new skt::Logger);
    logger->addAppender(skt::LogAppender::ptr(new skt::StdoutLogAppender));
    
    //skt::LogEvent::ptr event(new skt::LogEvent(__FILE__, __LINE__, 0, skt::GetThreadId(), 2, time(0)));
    //event -> getSS() << "Hello skt log";

    //logger->log(skt::LogLevel::DEBUG, event);
    std::cout << "hello! skt！" << std::endl;
    SKT_LOG_ERROR(logger) <<"test macro error";
    SKT_LOG_INFO(logger) << "test macro";
    return 0;
}
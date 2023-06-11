//
// Created by 25467 on 2023/6/11.
//

#include "iostream"
#include "../skt/log/log.h"
#include <ctime>

int main(int argc, char** argv){
    skt::Logger::ptr logger(new skt::Logger);
    logger->addAppender(skt::LogAppender::ptr(new skt::StdoutLogAppender));
    skt::LogEvent::ptr event(new skt::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    logger->log(skt::LogLevel::DEBUG, event);
    std::cout << "hello! 加油！" << std::endl;
    return 0;
}
//
// Created by 25467 on 2023/6/10.
//

#ifndef LEARNWEB_LOG_H
#define LEARNWEB_LOG_H

#include "string"
#include "inttypes.h"
#include "memory"
#include "list"
#include "sstream"
#include "fstream"
//m_level表示记录大于该level的日志

namespace skt{

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file = nullptr;  //文件名
    int32_t m_line = 0;      // 行号
    uint32_t m_elapse = 0;  //程序启动到现在的ms数
    int32_t m_threadId = 0; //线程id
    uint32_t m_fiberId = 0; //协程id
    uint64_t m_time;        //时间戳
    std::string m_content;  //日志内容
};

//日志级别
class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
};

//日志格式
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    std::string format(LogEvent::ptr event);
private:
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(); //因为可能需要设定输出地方

    void log(LogLevel::Level level, LogEvent::ptr event);

    void setFormatter(LogFormatter::ptr val){ m_formatter = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}

protected: //子类需要使用
    LogLevel::level m_level;
    LogFormatter::ptr m_formatter;
};


//日志输出器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const{ return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;}
private:
    std::string name;         //日志名称
    LogLevel::level m_level; //日志级别
    std::list<LogAppender::ptr> m_appenders;//Appender集合
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

//输出到文件的Appender
class FileLogAppender : public  LogAppender{
public:
    typedef  std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::String& filename);
    virtual void log(LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}


#endif //LEARNWEB_LOG_H

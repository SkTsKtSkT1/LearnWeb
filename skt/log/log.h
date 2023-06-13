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
#include "vector"
#include "map"
#include "thread"
#include "../util/util.h"
#include "../singleton/singleton.h"
//m_level表示记录大于该level的日志


//流式记录日志的宏
#define SKT_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        skt::LogEventWrap(skt::LogEvent::ptr(new skt::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, skt::GetThreadId(),\
                skt::GetFiberId(), time(0)))).getSS()


#define SKT_LOG_DEBUG(logger)  SKT_LOG_LEVEL(logger, skt::LogLevel::DEBUG)
#define SKT_LOG_INFO(logger)  SKT_LOG_LEVEL(logger, skt::LogLevel::INFO)
#define SKT_LOG_WARN(logger)  SKT_LOG_LEVEL(logger, skt::LogLevel::WARN)
#define SKT_LOG_ERROR(logger)  SKT_LOG_LEVEL(logger, skt::LogLevel::ERROR)
#define SKT_LOG_FATAL(logger)  SKT_LOG_LEVEL(logger, skt::LogLevel::FATAL)

//format记录日志的宏
#define SKT_LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if(logger->getLevel() <= level) \
    skt::LogEventWrap(skt::LogEvent::ptr(new skt::LogEvent(logger, level, \
                    __FILE__, __LINE__, 0, skt::GetThreadId(),\
                    skt::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define SKT_LOG_FMT_DEBUG(logger, fmt, ...)  SKT_LOG_FMT_LEVEL(logger, skt::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SKT_LOG_FMT_INFO(logger, fmt, ...)  SKT_LOG_FMT_LEVEL(logger, skt::LogLevel::INFO, fmt, __VA_ARGS__)
#define SKT_LOG_FMT_WARN(logger, fmt, ...)  SKT_LOG_FMT_LEVEL(logger, skt::LogLevel::WARN, fmt, __VA_ARGS__)
#define SKT_LOG_FMT_ERROR(logger, fmt, ...)  SKT_LOG_FMT_LEVEL(logger, skt::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SKT_LOG_FMT_FATAL(logger, fmt, ...)  SKT_LOG_FMT_LEVEL(logger, skt::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SKT_LOG_ROOT() skt:: LoggerMgr::GetInstance()->getRoot()


namespace skt{

class Logger;


//日志级别
class LogLevel{
public:
    enum Level{
        UNKOWN = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* ToString(LogLevel::Level level);
};

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
            const char* file, int32_t line, uint32_t elapse,
            uint32_t thread_id, uint32_t fiber_id, uint64_t time);

    const char* getFile() const {return m_file;}
    int32_t getLine() const {return m_line;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const {return m_threadId;}
    uint32_t getFiberId() const {return m_fiberId;}
    uint64_t getTime() const {return m_time;}
    std::string getContent() const {return m_ss.str();}
    std::shared_ptr<Logger> getLogger() const {return m_logger;}
    std::stringstream& getSS() {return m_ss;}
    LogLevel::Level getLevel() const {return m_level;}

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr;  //文件名
    int32_t m_line = 0;      // 行号
    uint32_t m_elapse = 0;  //程序启动到现在的ms数
    int32_t m_threadId = 0; //线程id
    uint32_t m_fiberId = 0; //协程id
    uint64_t m_time;        //时间戳
    std::stringstream m_ss;  //日志内容

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};

class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    
    LogEvent::ptr getEvent() const {return m_event;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;

};


//日志格式
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}

        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {}; //因为可能需要设定输出地方

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val){ m_formatter = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}

    LogLevel::Level getLevel() const {return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;} 

protected: //子类需要使用
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};


//日志输出器
class Logger : public std::enable_shared_from_this<Logger>{
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

    const std::string& getName() const {return m_name;}
private:
    std::string m_name;         //日志名称
    LogLevel::Level m_level; //日志级别
    std::list<LogAppender::ptr> m_appenders;//Appender集合
    LogFormatter::ptr m_formatter;
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

//输出到文件的Appender
class FileLogAppender : public  LogAppender{
public:
    typedef  std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

//管理所有的logger
class LoggerManager{
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const {return m_root;}
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef skt::Singleton<LoggerManager> LoggerMgr;

}


#endif //LEARNWEB_LOG_H

//
// Created by 25467 on 2023/6/10.
//

#include <iostream>
#include "log.h"
#include "map"
#include "functional"
#include "time.h"
#include "string"
#include "stdarg.h"
#include "../util/util.h"

namespace skt{
const char* LogLevel::ToString(LogLevel::Level level) {
    switch (level) {
#define XX(name) \
        case LogLevel::name: \
        return #name;\
        break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default:
            return "UNKONW";
    }
    return "UNKOWN";
}

LogEventWrap::LogEventWrap(LogEvent::ptr e) : m_event(e){

}

LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
        const char* file, int32_t line, uint32_t elapse,
        uint32_t thread_id, uint32_t fiber_id, uint64_t time) :
        m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_logger(logger)
        ,m_level(level){
}


void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
};

std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS();
}

void LogEvent::format(const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1){
        m_ss << std::string(buf, len);
        free(buf);
    }
}

Logger::Logger(const std::string &name) : m_name(name), m_level(LogLevel::DEBUG){
    //%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::addAppender(LogAppender::ptr appender) {
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);//保证每个都有format
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}


void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level){
        auto self = shared_from_this();
        for(auto& i : m_appenders){
            i -> log(self, level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level){
        std::cout << m_formatter->format(logger, level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename){}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level){
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream; //非0转变为1，0保持不变
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream  ss;
    for(auto& i : m_items){
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern){
    init();
}



class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string & str = ""){}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << logger->getName();
    }
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadId();
    }
};

class FiberIdFormatItem : public LogFormatter::FormatItem{
public:
    FiberIdFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberId();
    }
};


class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S") : m_format(format){
        if(m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    };
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string& str) : m_string(str){};
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << m_string;
    }
private:
    std::string m_string;
};

//输出Tab
class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str = "") : m_string(str){};
    void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << '\t';
    }
private:
    std::string m_string;
};


//%xxx  %xxx(xxx)    %% 转义为百分号
void LogFormatter::init() {
    //str format type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i = 0; i< m_pattern.size(); i++){
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()){
            if(m_pattern[i + 1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;

        while(n < m_pattern.size()){
            if(!fmt_status &&(!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')){//有空格表示已经结束了~
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout<< "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin =  n;
                    ++n;
                    continue;
                }
            }else if(fmt_status == 1){
                if(m_pattern[n] == '}'){ //格式结束了
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout<< "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()){
                if(str.empty()){
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0){//如果到最后都没有
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        }else if(fmt_status == 1){
            std::cout<<"pattern parse error: "<<m_pattern << " - "<< m_pattern.substr(i)<<std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> >  s_format_itmes = {
#define XX(str, C) \
            {#str, [](const std::string& fmt) {return FormatItem::ptr(new C(fmt));}}
                XX(m, MessageFormatItem), //%m 消息体
                XX(p, LevelFormatItem), //%p level
                XX(r, ElapseFormatItem), //%r 启动后的时间
                XX(c, NameFormatItem), //%c 日志名称
                XX(t, ThreadIdFormatItem), //%t 线程id
                XX(n, NewLineFormatItem),    //%n 回车换行
                XX(d, DateTimeFormatItem), //%d 时间
                XX(f, FileNameFormatItem), //%f 文件名
                XX(l, LineFormatItem), //%l 行数
                XX(T, TabFormatItem), //%T table
                XX(F, FiberIdFormatItem), //%F fiberId
#undef  XX
    };   
    for(auto& i :vec){
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_itmes.find(std::get<0>(i));
            if(it == s_format_itmes.end()) {
                m_items.push_back(
                    FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }else{
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout<< "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;
}


LoggerManager::LoggerManager() {
    m_root.reset(new Logger);
    //默认的appender
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
};

Logger::ptr  LoggerManager::getLogger(const std::string& name){
    auto it = m_loggers.find(name);
    return it == m_loggers.end() ? m_root : it->second;
}

}

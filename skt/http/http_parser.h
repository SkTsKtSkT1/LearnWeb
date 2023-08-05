#ifndef LEARNWEB_HTTP_PARSER_H
#define LEARNWEB_HTTP_PARSER_H


#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace skt{
namespace http{

class HttpRequestParser{
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
    HttpRequestParser();

    size_t excute(char* data, size_t len);
    int isFinished();
    int hasError();

    HttpRequest::ptr getData() const {return m_data;}
    void setError(int v) { m_error = v;}

    uint64_t getContentLength();

private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    int m_error;
};

class HttpResponseParser{
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
    HttpResponseParser();
    size_t excute(char* data, size_t len, bool chunk);
    int isFinished();
    int hasError();

    HttpResponse::ptr getData() const {return m_data;}
    void setError(int v) { m_error = v;}

    uint64_t getContentLength();
private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    //1000:invalid method
    //1001:invalid version
    //1002:
    int m_error;
};
}

}
#endif

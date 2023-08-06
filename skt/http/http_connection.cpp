#include "http_connection.h"
#include "http_parser.h"
#include "skt/log/log.h"

namespace skt{
namespace http{

static skt::Logger::ptr g_logger = SKT_LOG_NAME("system");
HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
        : SocketStream(sock, owner){

}

HttpResponse::ptr HttpConnection::recvResponse() {
    HttpResponseParser::ptr parser(new HttpResponseParser);

    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    std::shared_ptr<char> buff(new char[buff_size + 1], [](char *ptr) {
        delete[] ptr;
    });

    char *data = buff.get();
    int offset = 0;
    do {
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        data[len] = '\0';
        size_t nparse = parser->excute(data, len, false);//已处理的字节数且data有效数据为len - off

        if (parser->hasError()) {
            close();
            return nullptr;
        }
        offset = len - nparse;//有效的数据
        if (offset == (int) buff_size) { //进入这次循环说明上一次读没有结束，并且此次缓存区满了都不能读完 放弃（index 0-->buff_size - 1
            close();
            return nullptr;
        }
        if (parser->isFinished()) {
            break;
        }
    } while (true);
    auto& client_parser = parser->getParser();
    if (client_parser.chunked) {
        int len = offset;
        std::string body;
        do{
            do{
                int rt = read(data + len, buff_size - len);
                if(rt <= 0) {
                    close();
                    return nullptr;
                }
                len += rt;
                data[len] = '\0';
                size_t nparse = parser->excute(data, len, true);
                if(parser->hasError()){
                    close();
                    return nullptr;
                }
                len -= nparse;
                if(len == (int)buff_size) {
                    close();
                    return nullptr;
                }
            }while(!parser->isFinished());
            len -= 2;
            SKT_LOG_INFO(g_logger) << "content_len =" << client_parser.content_len;
            if(client_parser.content_len <= len){
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len, len - client_parser.content_len);
                len -= client_parser.content_len;
            }else{
                body.append(data, len);
                int left = client_parser.content_len - len;
                while(left > 0){
                    int rt = read(data, left > (int)buff_size ? (int)buff_size : left);
                    if(rt <= 0){
                        close();
                        return nullptr;
                    }
                    body.append(data, rt);
                    left -= rt;
                }
                len = 0;
            }
        }while(!client_parser.chunks_done);
        parser->getData()->setBody(body);
    } else {
        int64_t length = parser->getContentLength();
        std::string body;
        //store
        if (length > 0) {
            body.resize(length);

            int len = 0;
            if (length >= offset) {
                memcpy(&body[0], data, offset);
                len = offset;
            } else {
                memcpy(&body[0], data, length);
            }
            length -= offset;
            if (length > 0) {
                if (readFixSize(&body[len], length) <= 0) {
                    close();
                    return nullptr;
                }
            }
            parser->getData()->setBody(body);
        }
    }
//    if (!body.empty()) {
//        parser->getData()->setBody(body);
//    }
    //parser->getData()->init();
    return parser->getData();
}

int HttpConnection::sendRequest(HttpRequest::ptr rsp) {
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}


}
}
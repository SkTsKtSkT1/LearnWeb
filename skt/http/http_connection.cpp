#include "http_connection.h"
#include "http_parser.h"

namespace skt{
namespace http{
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
        size_t nparse = parser->excute(data, len, true);//已处理的字节数且data有效数据为len - off

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
    auto client_parser = parser->getParser();
    std::string body;
    if (client_parser.chunked) {
        //int len = offset;
        do{
            break;
        }while(!parser->isFinished());
    } else {
        int64_t length = parser->getContentLength();
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
        }
    }
    if (!body.empty()) {
        parser->getData()->setBody(body);
    }
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
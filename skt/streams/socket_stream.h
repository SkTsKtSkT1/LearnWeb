#ifndef LEARNWEB_SOCKET_STREAM_H
#define LEARNWEB_SOCKET_STREAM_H

#include "skt/stream/stream.h"
#include "skt/socket/socket.h"


namespace skt{
class SocketStream : public Stream{
public:
    typedef std::shared_ptr<SocketStream> ptr;
    //if true ~SocketStream will close the socket
    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();

    virtual int read(void* buffer, size_t length) override;
    virtual int read(ByteArray::ptr ba, size_t length) override;
    virtual int write(const void* buffer, size_t length) override;
    virtual int write(ByteArray::ptr ba, size_t length) override;
    virtual void close() override;

    Socket::ptr getSocket() const { return m_socket;}
    bool isConnected() const;
private:
    Socket::ptr m_socket;
    bool m_owner;
};

}


#endif
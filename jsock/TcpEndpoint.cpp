#include "TcpEndpoint.h"
#include <sys/socket.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <cassert>

namespace jsock {

TcpEndpoint::TcpEndpoint(const Name& destination):
        socket(STREAM) {
    sockaddr_in dst = destination;
    if (connect(this->socket,
            (sockaddr*)&dst, sizeof(dst)) < 0)
        throw SocketException(errno);
}

TcpEndpoint::TcpEndpoint(int fileDescriptor):
        socket(fileDescriptor) { }

TcpEndpoint::~TcpEndpoint() {
    shutdown(this->socket, SHUT_RDWR);
}

void TcpEndpoint::write(
        const std::vector<unsigned char>& data) const {
    this->socket.throwIfError();
    if (send(this->socket, &data[0],
            data.size(), MSG_DONTWAIT) < 0)
        throw SocketException(errno);
}

std::vector<unsigned char> TcpEndpoint::read() const {
    this->socket.throwIfError();
    int readSize = recv(this->socket, (void*)this->buffer,
            TcpEndpoint::MAX_READ_SIZE, MSG_DONTWAIT);
    if (readSize < 0) {
        if (errno != EAGAIN)
        if (errno != EWOULDBLOCK)
            throw SocketException(errno);
    }
    return std::vector<unsigned char>(this->buffer,
        this->buffer + std::max(readSize, 0));
}

Name TcpEndpoint::getRemoteName() const {
    sockaddr_in name;
    socklen_t size = sizeof(name);
    if (getsockname(this->socket, (sockaddr*)&name,
        &size) < 0) throw SocketException(errno);
    return Name(name);
}

Name TcpEndpoint::getLocalName() const {
    sockaddr_in name;
    socklen_t size = sizeof(name);
    if (getpeername(this->socket, (sockaddr*)&name,
        &size) < 0) throw SocketException(errno);
    return Name(name);
}

}; //namespace jsock

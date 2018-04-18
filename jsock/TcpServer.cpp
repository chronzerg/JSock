#include "TcpEndpoint.h"
#include "TcpServer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace jsock {

TcpServer::TcpServer(unsigned int port): socket(STREAM) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->socket, (sockaddr*)&address,
        sizeof(address)) < 0) throw SocketException(errno);

    if (listen(this->socket, 10) < 0)
        throw SocketException(errno);

    int flags = fcntl(this->socket, F_GETFL, 0);
    if (flags < 0) throw SocketException(errno);
    int errors = fcntl(this->socket, F_SETFL, flags | O_NONBLOCK);
    if (errors < 0) throw SocketException(errno);
}

std::unique_ptr<Endpoint> TcpServer::accept() const {
    sockaddr_in address;
    socklen_t length = sizeof(address);
    int fileDescriptor = ::accept(this->socket,
        (sockaddr*)&address, &length);
    if (fileDescriptor < 0)
        if (errno != EAGAIN)
        if (errno != EWOULDBLOCK)
            throw SocketException(errno);
    return std::unique_ptr<Endpoint>(
            fileDescriptor < 0 ? NULL :
            new TcpEndpoint(fileDescriptor));
}

} // namespace jsock

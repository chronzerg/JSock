#include "TcpServer.h"
#include <sys/socket.h>
#include <arpa/inet.h>

namespace jsock {

TcpServer::TcpServer(unsigned int port) {
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->socket, (struct sockaddr*)&address,
		sizeof(address) < 0)) throw SocketException(errno);

	if (listen(this->socket, 10) < 0)
		throw SocketException(errno);
}

TcpClient TcpServer::accept() const {
	struct sockaddr_in address;
	socklen_t length;
	int fileDescriptor = ::accept(this->socket,
		(struct sockaddr*)&address, &length);
	if (fileDescriptor < 0) throw SocketException(errno);
	return TcpClient(StreamSocket(fileDescriptor));
}

} // namespace jsock

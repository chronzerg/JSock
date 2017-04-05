#include "TcpEndpoint.h"
#include <sys/socket.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

namespace jsock {

TcpEndpoint::TcpEndpoint(const Authority& destination):
		socket(stream), remote(destination) {
	struct sockaddr_in dest = destination;
	if(connect(this->socket, (struct sockaddr*)&dest,
		sizeof(dest)) < 0) throw SocketException(errno);
}

TcpEndpoint::TcpEndpoint(int fileDescriptor):
	socket(fileDescriptor) {}

TcpEndpoint::~TcpEndpoint() {
	shutdown(this->socket, SHUT_RDWR);
}

void TcpEndpoint::write(const std::vector<unsigned char>& data) const {
	this->socket.throwIfError();
	if(send(this->socket, &data[0], data.size(), MSG_DONTWAIT) < 0)
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

}; //namespace jsock

#include "TcpEndpoint.h"
#include <sys/socket.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <cassert>

namespace jsock {

TcpEndpoint::TcpEndpoint(const Name& destination):
		socket_(stream) {
	struct sockaddr_in dst = destination;
	if(connect(this->socket_, (struct sockaddr*)&dst,
		sizeof(dst)) < 0) throw SocketException(errno);
}

TcpEndpoint::TcpEndpoint(int fileDescriptor):
		socket_(fileDescriptor) { }

TcpEndpoint::~TcpEndpoint() {
	shutdown(this->socket_, SHUT_RDWR);
}

void TcpEndpoint::write(const std::vector<unsigned char>& data) const {
	this->socket_.throwIfError();
	if(send(this->socket_, &data[0], data.size(), MSG_DONTWAIT) < 0)
		throw SocketException(errno);
}

std::vector<unsigned char> TcpEndpoint::read() const {
	this->socket_.throwIfError();
	int readSize = recv(this->socket_, (void*)this->buffer,
			TcpEndpoint::MAX_READ_SIZE, MSG_DONTWAIT);
	if (readSize < 0) {
		if (errno != EAGAIN)
		if (errno != EWOULDBLOCK)
			throw SocketException(errno);
	}
	return std::vector<unsigned char>(this->buffer,
		this->buffer + std::max(readSize, 0));
}

Name TcpEndpoint::getName(Side side) const {
	struct sockaddr_in name;
	socklen_t size = sizeof(name);
	if(side == Local) {
		if(getsockname(this->socket_, (struct sockaddr*)&name,
			&size) < 0) throw SocketException(errno);
	}
	else {
		assert(side == Remote);
		if(getpeername(this->socket_, (struct sockaddr*)&name,
			&size) < 0) throw SocketException(errno);
	}
	return Name(name);
}

}; //namespace jsock

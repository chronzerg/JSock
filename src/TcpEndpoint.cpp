#include "TcpEndpoint.h"
#include <sys/socket.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

namespace jsock {

TcpEndpoint::TcpEndpoint(const Authority& destination):
		socket_(stream), remote_(new Authority(destination)) {
	struct sockaddr_in dst = destination;
	if(connect(this->socket_, (struct sockaddr*)&dst,
		sizeof(dst)) < 0) throw SocketException(errno);

	struct sockaddr_in local;
	socklen_t size = sizeof(local);
	if(getsockname(this->socket_, (struct sockaddr*)&local,
		&size) < 0) throw SocketException(errno);
	local_ = std::unique_ptr<Authority>(new Authority(local));
}

TcpEndpoint::TcpEndpoint(int fileDescriptor):
		socket_(fileDescriptor) {
	struct sockaddr_in remote;
	socklen_t rsize = sizeof(remote);
	if(getpeername(this->socket_, (struct sockaddr*)&remote,
		&rsize) < 0) throw SocketException(errno);
	local_ = std::unique_ptr<Authority>(new Authority(remote));


	struct sockaddr_in local;
	socklen_t lsize = sizeof(local);
	if(getsockname(this->socket_, (struct sockaddr*)&local,
		&lsize) < 0) throw SocketException(errno);
	local_ = std::unique_ptr<Authority>(new Authority(local));
}

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

Authority TcpEndpoint::remote() const {
	return *(this->remote_);
}

Authority TcpEndpoint::local() const {
	return *(this->local_);
}

}; //namespace jsock

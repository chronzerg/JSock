#include "SocketException.h"
#include "StreamSocket.h"
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <errno.h>

namespace jsock {

const unsigned int errorStringLength = 50;

StreamSocket::StreamSocket() {
	this->fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fileDescriptor < 0) {
		char errorString[errorStringLength];
		strerror_r(errno, errorString, errorStringLength);
		throw std::runtime_error(std::string(errorString));
	}
}

StreamSocket::~StreamSocket() {
	close(this->fileDescriptor);
}

StreamSocket::operator int() const {
	return this->fileDescriptor;
}

void StreamSocket::throwIfError() const {
	int error = 0;
	socklen_t errorSize = sizeof(error);
	int getError = getsockopt(*this,
			SOL_SOCKET, SO_ERROR, &error, &errorSize);
	if (getError != 0) {
		throw SocketException(errno);
	}
	if (error != 0) {
		throw SocketException(error);
	}
}

} // namespace jsock

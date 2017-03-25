#include "StreamSocket.h"
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <errno.h>

namespace raiisocket {

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

StreamSocket::operator int() {
	return this->fileDescriptor;
}

} // namespace raiisocket

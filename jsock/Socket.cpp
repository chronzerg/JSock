#include "Socket.h"
#include "SocketException.h"

#include <errno.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace jsock {

Socket::Socket(SocketType type) : fileDescriptor(socket(AF_INET, type, 0)) {
  if (this->fileDescriptor < 0) {
    throw SocketException(errno);
  }
}

Socket::Socket(int fileDescriptor) : fileDescriptor(fileDescriptor) {}

Socket::~Socket() { close(this->fileDescriptor); }

Socket::operator int() const { return this->fileDescriptor; }

void Socket::throwIfError() const {
  int error = 0;
  socklen_t errorSize = sizeof(error);
  // TODO - Is it a problem that we are exposing errno
  // and the return error via the same interface?
  int getError = getsockopt(*this, SOL_SOCKET, SO_ERROR, &error, &errorSize);
  if (getError != 0) {
    throw SocketException(errno);
  }
  if (error != 0) {
    throw SocketException(error);
  }
}

} // namespace jsock

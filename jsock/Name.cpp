#include "Name.h"

namespace jsock {

std::string getAddress(sockaddr_in name) {
  char address[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &name.sin_addr, address, INET_ADDRSTRLEN) == NULL)
    throw SocketException(errno);
  return std::string(address);
}

unsigned int getPort(sockaddr_in name) { return ntohs(name.sin_port); }

Name::Name(const std::string &address, unsigned int port)
    : address(address), port(port) {}

Name::Name(const sockaddr_in &name)
    : address(getAddress(name)), port(getPort(name)) {}

Name::operator sockaddr_in() const {
  sockaddr_in sockStruct;
  sockStruct.sin_family = AF_INET;
  sockStruct.sin_port = htons(this->port);

  int error = inet_pton(AF_INET, this->address.c_str(), &(sockStruct.sin_addr));
  if (error == 0) {
    // TODO - Think of a better exception, one that can be
    // programmatically checked.
    std::string errorString("Invalid IP address: ");
    errorString += this->address;
    throw SocketException(errorString);
  } else if (error < 0)
    throw SocketException(errno);
  return sockStruct;
}

Name::operator std::string() const {
  std::stringstream stream;
  stream << this->address << ":" << this->port;
  return stream.str();
}

} // namespace jsock

#pragma once

#include "SocketException.h"
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include <sys/socket.h>

namespace jsock {

// Represents an IP address and port pair. Facilitates converting
// said pair from human readable to OS readable form.
class Name {
public:
  const std::string address;
  const unsigned int port;

  Name(const std::string &address, unsigned int port);
  Name(const sockaddr_in &cstruct);

  operator sockaddr_in() const;
  operator std::string() const;
};

} // namespace jsock

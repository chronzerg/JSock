#pragma once

#include "jsock/Endpoint.h"
#include <memory>
#include <vector>

// Collects message fragments from the underlying
// endpoint and provides them to the caller as a
// completed message.
class ParsingEndpoint : public virtual jsock::Endpoint {
private:
  std::unique_ptr<jsock::Endpoint> socket;
  std::unique_ptr<std::vector<unsigned char>> buffer;

public:
  ParsingEndpoint(std::unique_ptr<jsock::Endpoint> socket);

  // Passthrough to the underlying socket
  void write(const std::vector<unsigned char> &data) const;

  // Reads data from the socket and appends it to the running
  // data buffer. If at least one message is completed, return
  // it/them to the caller. Otherwise, return an empty vector.
  std::vector<unsigned char> read() const;

  jsock::Name getName(Side side) const;
};

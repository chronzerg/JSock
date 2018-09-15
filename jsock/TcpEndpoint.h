#ifndef TcpEndpoint_h
#define TcpEndpoint_h

#include "Endpoint.h"
#include "Socket.h"
#include "SocketException.h"
#include <string>
#include <vector>

namespace jsock {

// A connected TCP pipe
class TcpEndpoint : public virtual Endpoint {
private:
  const static unsigned int MAX_READ_SIZE = 256;

  const Socket socket;
  unsigned char buffer[TcpEndpoint::MAX_READ_SIZE];

public:
  TcpEndpoint(const Name &destination);
  TcpEndpoint(int fileDescriptor);
  ~TcpEndpoint();

  void write(const std::vector<unsigned char> &data) const;
  std::vector<unsigned char> read() const;
  Name getRemoteName() const;
  Name getLocalName() const;
};

} // namespace jsock

#endif // TcpEndpoint_h

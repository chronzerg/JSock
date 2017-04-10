#ifndef TcpEndpoint_h
#define TcpEndpoint_h

#include "SocketException.h"
#include "Endpoint.h"
#include "Socket.h"
#include <vector>
#include <string>

namespace jsock {

// A connected TCP pipe
class TcpEndpoint: public virtual Endpoint {
private:
	Socket socket_;
	const static unsigned int MAX_READ_SIZE = 256;
	unsigned char buffer[TcpEndpoint::MAX_READ_SIZE];

public:
	TcpEndpoint(const Name& destination);
	TcpEndpoint(int fileDescriptor);
	TcpEndpoint(const TcpEndpoint& other) = delete;
	~TcpEndpoint();

	void write(const std::vector<unsigned char>&) const;
	std::vector<unsigned char> read() const;
	Name getName(Side side) const;
};

} // namespace jsock

#endif // TcpEndpoint_h

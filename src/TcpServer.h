#ifndef TcpServer_h
#define TcpServer_h

#include "TcpEndpoint.h"
#include "Socket.h"

namespace jsock {

class TcpServer {
	private:
		Socket socket;

	public:
		TcpServer(unsigned int port);
		TcpEndpoint accept() const;
};

} // namespace jsock

#endif // TcpServer_h

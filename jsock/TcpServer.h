#ifndef TcpServer_h
#define TcpServer_h

#include "TcpEndpoint.h"
#include "Socket.h"
#include <memory>

namespace jsock {

class TcpServer {
	private:
		Socket socket;

	public:
		TcpServer(unsigned int port);
		std::unique_ptr<Endpoint> accept() const;
};

} // namespace jsock

#endif // TcpServer_h

#ifndef TcpServer_h
#define TcpServer_h

#include "TcpClient.h"

namespace jsock {

class TcpServer {
	private:
		StreamSocket socket;

	public:
		TcpServer(unsigned int port);
		TcpClient accept() const;
};

} // namespace jsock

#endif // TcpServer_h

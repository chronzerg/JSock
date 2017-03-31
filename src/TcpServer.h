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

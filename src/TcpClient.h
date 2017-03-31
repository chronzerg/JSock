#ifndef TcpClient_h
#define TcpClient_h

#include "SocketException.h"
#include "StreamSocket.h"
#include <vector>
#include <string>

namespace jsock {


class TcpClient {
	private:
		StreamSocket socket;
		const static unsigned int MAX_READ_SIZE = 256;
		unsigned char buffer[TcpClient::MAX_READ_SIZE];

	public:
		TcpClient(const std::string&, unsigned short);
		TcpClient(StreamSocket socket);
		~TcpClient();
		void write(const std::vector<unsigned char>&) const;
		std::vector<unsigned char> read() const;
};

} // namespace jsock

#endif // TcpClient_h

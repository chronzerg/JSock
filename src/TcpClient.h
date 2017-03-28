#ifndef TcpClient_h
#define TcpClient_h

#include "SocketException.h"
#include "StreamSocket.h"
#include <vector>
#include <string>

namespace raiisocket {


class TcpClient {
	private:
		StreamSocket socket;
		const static unsigned int MAX_READ_SIZE = 256;
		unsigned char buffer[TcpClient::MAX_READ_SIZE];

	public:
		TcpClient(const std::string&, unsigned short);
		~TcpClient();
		void write(const std::vector<unsigned char>&);
		std::vector<unsigned char> read();
};

} // namespace raiisocket

#endif // TcpClient_h

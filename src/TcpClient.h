#include "StreamSocket.h"
#include <sys/socket.h>
#include <netinet/ip.h>
#include <vector>
#include <string>

namespace raiisocket {


class TcpClient {
	private:
		StreamSocket socket;
		const static unsigned int MAX_READ_SIZE = 256;
		unsigned char buffer[TcpClient::MAX_READ_SIZE];

	public:
		TcpClient(const std::string&, unsigned int);
		~TcpClient();
		void write(const std::vector<unsigned char>&);
		std::vector<unsigned char> read();
};

} // namespace raiisocket

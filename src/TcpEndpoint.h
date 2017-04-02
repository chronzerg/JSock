#ifndef TcpEndpoint_h
#define TcpEndpoint_h

#include "SocketException.h"
#include "Endpoint.h"
#include "Socket.h"
#include <vector>
#include <string>

namespace jsock {

class TcpEndpoint: public virtual Endpoint {
	private:
		Socket socket;
		const static unsigned int MAX_READ_SIZE = 256;
		unsigned char buffer[TcpEndpoint::MAX_READ_SIZE];

	public:
		TcpEndpoint(const std::string&, unsigned short);
		TcpEndpoint(int fileDescriptor);
		~TcpEndpoint();
		void write(const std::vector<unsigned char>&) const;
		std::vector<unsigned char> read() const;
};

} // namespace jsock

#endif // TcpEndpoint_h

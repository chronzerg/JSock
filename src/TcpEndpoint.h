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
		Socket socket_;
		std::unique_ptr<Authority> remote_;
		std::unique_ptr<Authority> local_;

		const static unsigned int MAX_READ_SIZE = 256;
		unsigned char buffer[TcpEndpoint::MAX_READ_SIZE];

	public:
		TcpEndpoint(const Authority& destination);
		TcpEndpoint(int fileDescriptor);
		~TcpEndpoint();
		void write(const std::vector<unsigned char>&) const;
		std::vector<unsigned char> read() const;
		Authority remote() const;
		Authority local() const;
};

} // namespace jsock

#endif // TcpEndpoint_h

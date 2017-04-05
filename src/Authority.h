#ifndef Authority_h
#define Authority_h

#include "SocketException.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

namespace jsock {

// Represents an IP address and port pair. Facilitates converting
// said pair from human readable to OS readable form.
class Authority {
	private:
		struct sockaddr_in _sockStruct;
		std::string _address;
		unsigned int _port;

		void parse() {
			this->_sockStruct.sin_family = AF_INET;
			this->_sockStruct.sin_port = htons(this->_port);

			int error = inet_pton(AF_INET, this->_address.c_str(),
					&(this->_sockStruct.sin_addr));
			if(error == 0) {
				// TODO - Think of a better exception, one that can be
				// programmatically checked.
				std::string errorString("Invalid IP address: ");
				errorString += this->_address;
				throw SocketException(errorString);
			}
			else if (error < 0)
				throw SocketException(errno);
		}

	public:
		Authority(const std::string& address, unsigned int port):
				_address(address), _port(port) {
			parse();
		}

		Authority(const Authority& other):
				_address(other.address()), _port(other.port()) {
			parse();
		}

		Authority(const struct sockaddr_in& cstruct) {
			this->_sockStruct = cstruct;
			this->_port = ntohs(cstruct.sin_port);

			char buffer[INET_ADDRSTRLEN];
			if(inet_ntop(AF_INET, &cstruct.sin_addr,
				(char*)&buffer, INET_ADDRSTRLEN) == NULL)
					throw SocketException(errno);
			this->_address = std::string(buffer);
		}

		std::string address() const { return this->_address; }
		unsigned int port() const { return this->_port; }

		operator struct sockaddr_in() const {
			return this->_sockStruct;
		}
};

} // namespace jsock

#endif // Authority_h

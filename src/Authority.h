#ifndef Authority_h
#define Authority_h

#include "SocketException.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>

namespace jsock {

// Represents an IP address and port pair. Facilitates converting
// said pair from human readable to OS readable form.
class Authority {
	private:
		struct sockaddr_in sockStruct_;
		std::string address_;
		unsigned int port_;
		std::string str_;

		void parse() {
			this->sockStruct_.sin_family = AF_INET;
			this->sockStruct_.sin_port = htons(this->port_);

			int error = inet_pton(AF_INET, this->address_.c_str(),
					&(this->sockStruct_.sin_addr));
			if(error == 0) {
				// TODO - Think of a better exception, one that can be
				// programmatically checked.
				std::string errorString("Invalid IP address: ");
				errorString += this->address_;
				throw SocketException(errorString);
			}
			else if (error < 0)
				throw SocketException(errno);
		}

		void stringify() {
			std::stringstream stream;
			stream << this->address_ << ":"  << this->port_;
			str_ = stream.str();
		}

	public:
		Authority(const std::string& address, unsigned int port):
				address_(address), port_(port) {
			parse();
			stringify();
		}

		Authority(const Authority& other):
				address_(other.address()), port_(other.port()) {
			parse();
			stringify();
		}

		Authority(const struct sockaddr_in& cstruct) {
			this->sockStruct_ = cstruct;
			this->port_ = ntohs(cstruct.sin_port);

			char buffer[INET_ADDRSTRLEN];
			if(inet_ntop(AF_INET, &cstruct.sin_addr,
				buffer, INET_ADDRSTRLEN) == NULL)
					throw SocketException(errno);
			this->address_ = std::string(buffer);
			stringify();
		}

		std::string address() const { return this->address_; }
		unsigned int port() const { return this->port_; }

		operator struct sockaddr_in() const {
			return this->sockStruct_;
		}

		operator std::string() const {
			return this->str_;
		}
};

} // namespace jsock

#endif // Authority_h

#ifndef Name_h
#define Name_h

#include "SocketException.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <string>

namespace jsock {

// Represents an IP address and port pair. Facilitates converting
// said pair from human readable to OS readable form.
class Name {
	private:
		static std::string getAddress(struct sockaddr_in name);
		static unsigned int getPort(struct sockaddr_in name);

	public:
		const std::string address;
		const unsigned int port;
		Name(const std::string& address, unsigned int port);
		Name(const struct sockaddr_in& cstruct);
		operator struct sockaddr_in() const;
		operator std::string() const;
};

} // namespace jsock

#endif // Name_h

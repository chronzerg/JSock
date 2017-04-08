#ifndef StreamSocket_h
#define StreamSocket_h

#include <sys/socket.h>

namespace jsock {

enum SocketType {
	datagram = SOCK_DGRAM,
	stream = SOCK_STREAM
};

class Socket {
	private:
		int fileDescriptor;

	public:
		Socket(SocketType type);
		Socket(int fileDescriptor);
		~Socket();
		operator int() const;
		void throwIfError() const;
};

} // namespace jsock

#endif // StreamSocket_h

#ifndef StreamSocket_h
#define StreamSocket_h

#include <sys/socket.h>

namespace jsock {

enum SocketType {
    DATAGRAM = SOCK_DGRAM,
    STREAM = SOCK_STREAM
};

class Socket {
private:
    const int fileDescriptor;

public:
    Socket(SocketType type);
    Socket(int fileDescriptor);
    ~Socket();

    operator int() const;
    void throwIfError() const;
};

} // namespace jsock

#endif // StreamSocket_h

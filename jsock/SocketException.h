#ifndef SocketException_h
#define SocketException_h

#include <exception>
#include <string.h>
#include <sstream>
#include <string>

namespace jsock {

class SocketException: public std::exception {
public:
    SocketException(const std::string& message);
    SocketException(int errorNumber);
    ~SocketException() throw();

    virtual const char* what() const throw();

    const std::string message;
    const int errorNumber;
};

} // namespace jsock

#endif // SocketException_h

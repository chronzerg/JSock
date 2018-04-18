#include "SocketException.h"

namespace jsock {

std::string createMessage(int errorNumber) {
    std::stringstream errorStream;
    errorStream << "Error " << errorNumber;
    return errorStream.str();
}

SocketException::SocketException(const std::string& message):
        message(message),
        errorNumber(1) { }

SocketException::SocketException(int errorNumber):
        message(createMessage(errorNumber)),
        errorNumber(errorNumber) { }

SocketException::~SocketException() throw() { }

const char* SocketException::what() const throw() {
    return message.c_str();
}

} // namespace jsock

#ifndef SocketException_h
#define SocketException_h

#include <exception>
#include <string.h>
#include <sstream>
#include <string>

namespace raiisocket {

struct SocketException : public std::exception {
	private:
		static const unsigned int ERR_STR_SIZE = 50;
		std::string message;

	public:
		SocketException(const std::string& message):
			message(message) {}

		SocketException(int errorNumber) {
			std::stringstream errorStream;
			errorStream << "SOCKET ERROR " << errno
				<< ": " << strerror(errorNumber);
			this->message = errorStream.str();
		}

		~SocketException() throw () {}

		const char* what() const throw()
			{ return message.c_str(); }
};

} // namespace raiisocket

#endif // SocketException_h

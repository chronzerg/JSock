#ifndef SocketException_h
#define SocketException_h

#include <exception>
#include <string.h>
#include <sstream>
#include <string>

namespace jsock {

struct SocketException: public std::exception {
	private:
		std::string message;
		int errorNumber_;

	public:
		SocketException(const std::string& message):
                message(message) {
            errorNumber_ = 1;
        }

		SocketException(int errorNumber) {
			this->errorNumber_ = errorNumber;
			std::stringstream errorStream;
			errorStream << "Error " << errorNumber;
			SocketException(errorStream.str());
		}

		~SocketException() throw () {}

		const char* what() const throw() {
			return message.c_str();
		}

		int errorNumber() const {
			return this->errorNumber_;
		}
};

} // namespace jsock

#endif // SocketException_h

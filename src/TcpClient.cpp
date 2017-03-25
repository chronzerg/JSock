#include "TcpClient.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <errno.h>

namespace raiisocket {

const unsigned int errorStringLength = 50;

TcpClient::TcpClient(const std::string& address, unsigned int port) {
	struct sockaddr_in addressStruct;

	{ // Set the IP address
		int error = inet_pton(AF_INET, address.c_str(), &(addressStruct.sin_addr));
		if (error == 0) {
			std::string message("Invalid IP address: ");
			message += address;
			std::cerr << message << std::endl;
			throw message.c_str();
		}
		else if (error < 0) {
			char errorString[errorStringLength];
			strerror_r(errno, errorString, errorStringLength);
			std::cerr << errorString << std::endl;
			throw errorString;
		}
	}

	{ // Set the port
		addressStruct.sin_port = port;
	}

	{ // Connect the socket 
		int error = connect(this->socket, (struct sockaddr*)&addressStruct, sizeof(addressStruct));
		if (error < 0) {
			char errorString[errorStringLength];
			strerror_r(errno, errorString, errorStringLength);
			throw errorString;
		}
	}
}

TcpClient::~TcpClient() {
	shutdown(this->socket, SHUT_RDWR);
}

void TcpClient::write(const std::vector<unsigned char>& data) {
	send(this->socket, &data[0], data.size(), 0);
}

// She's beautiful...
std::vector<unsigned char> TcpClient::read() {
	return std::vector<unsigned char>(this->buffer,
			this->buffer + recv(this->socket, 
				this->buffer, sizeof(this->buffer),
					TcpClient::MAX_READ_SIZE));
}

}; //namespace raiisocket

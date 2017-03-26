#include "TcpClient.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <errno.h>

namespace raiisocket {

TcpClient::TcpClient(const std::string& address, unsigned short port) {
	struct sockaddr_in addressStruct;
	addressStruct.sin_family = AF_INET;
	addressStruct.sin_port = htons(port);

	// Set the IP address
	int error = inet_pton(AF_INET, address.c_str(),
			&(addressStruct.sin_addr));
	if(error == 0) {
		std::string errorString("Invalid IP address: ");
		errorString += address;
		throw SocketException(errorString);
	}
	else if (error < 0)
		throw SocketException(errno);

	// Connect the socket 
	if(connect(this->socket, (struct sockaddr*)&addressStruct,
			sizeof(addressStruct)) < 0) throw SocketException(errno);
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
				this->buffer, TcpClient::MAX_READ_SIZE, 0));
}

}; //namespace raiisocket

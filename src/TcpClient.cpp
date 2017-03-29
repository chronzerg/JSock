#include "TcpClient.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

namespace jsock {

TcpClient::TcpClient(const std::string& address,
		unsigned short port) {
	struct sockaddr_in addressStruct;
	addressStruct.sin_family = AF_INET;
	addressStruct.sin_port = htons(port);

	int error = inet_pton(AF_INET, address.c_str(),
			&(addressStruct.sin_addr));
	if(error == 0) {
		std::string errorString("Invalid IP address: ");
		errorString += address;
		throw SocketException(errorString);
	}
	else if (error < 0)
		throw SocketException(errno);

	if(connect(this->socket, (struct sockaddr*)&addressStruct,
		sizeof(addressStruct)) < 0) throw SocketException(errno);

	int flags = fcntl(this->socket, F_GETFL, 0);
	if (flags < 0) throw SocketException(errno);
	int result = fcntl(this->socket, F_SETFL, flags | O_NONBLOCK);
	if (result < 0) throw SocketException(errno);
}

TcpClient::~TcpClient() {
	shutdown(this->socket, SHUT_RDWR);
}

void TcpClient::write(const std::vector<unsigned char>& data) {
	this->socket.throwIfError();
	if(send(this->socket, &data[0], data.size(), 0) < 0)
		throw SocketException(errno);
}

std::vector<unsigned char> TcpClient::read() {
	this->socket.throwIfError();
	int readSize = recv(this->socket, this->buffer,
			TcpClient::MAX_READ_SIZE, 0);
	if (readSize < 0) {
		if (errno != EWOULDBLOCK)
		if (errno != EAGAIN)
			throw SocketException(errno);
	}
	return std::vector<unsigned char>(this->buffer,
		this->buffer + readSize);
}

}; //namespace jsock

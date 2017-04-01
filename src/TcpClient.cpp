#include "TcpClient.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <algorithm>
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
}

TcpClient::TcpClient(int fileDescriptor):
	socket(fileDescriptor) {}

TcpClient::~TcpClient() {
	shutdown(this->socket, SHUT_RDWR);
}

void TcpClient::write(const std::vector<unsigned char>& data) const {
	this->socket.throwIfError();
	if(send(this->socket, &data[0], data.size(), MSG_DONTWAIT) < 0)
		throw SocketException(errno);
}

std::vector<unsigned char> TcpClient::read() const {
	this->socket.throwIfError();
	int readSize = recv(this->socket, (void*)this->buffer,
			TcpClient::MAX_READ_SIZE, MSG_DONTWAIT);
	if (readSize < 0) {
		if (errno != EWOULDBLOCK)
		if (errno != EAGAIN)
			throw SocketException(errno);
	}
	return std::vector<unsigned char>(this->buffer,
		this->buffer + std::max(readSize, 0));
}

}; //namespace jsock

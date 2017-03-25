#include "../src/TcpClient.h"
#include <vector>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>

int main() {
	try {
		raiisocket::TcpClient socket(std::string("127.0.0.1"), 5445);	
		while(true) {
			std::vector<unsigned char> message = socket.read();
			printf("%s", &message[0]);
		}
	}
	catch(const char* problem) {
		std::cout << "FATAL ERROR" << std::endl
			<< problem << std::endl;
	}
}

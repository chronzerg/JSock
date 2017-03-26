#include "../src/SocketException.h"
#include "../src/TcpClient.h"
#include <exception>
#include <iostream>
#include <vector>
#include <string>

int main() {
	try {
		raiisocket::TcpClient socket("127.0.0.1", 1234);	
		std::cout << "Enter input. Written after every newline."
			<< std::endl;
		for(std::string input; std::getline(std::cin, input);) {
			std::vector<unsigned char> data(&input[0],
					&input[0] + input.size());
			socket.write(data);
		}
	}
	catch(const raiisocket::SocketException& problem) {
		std::cerr << problem.what() << std::endl;
	}
}

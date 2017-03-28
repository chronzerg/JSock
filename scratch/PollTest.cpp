#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <cstdio>
#include <poll.h>

int main() {
	while(true) {
		struct pollfd options {
			.fd=STDIN_FILENO,
			.events = POLLIN
		};
		int result = poll(&options, 1, 500);
		if (result < -1)
			std::cout << "e" << errno << std::flush;
		else if (result == 0)
			std::cout << "_" << std::flush;
		else
			std::cout << "<" << getchar() << std::flush;
	}
}

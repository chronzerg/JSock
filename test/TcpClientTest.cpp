#include "../src/SocketException.h"
#include "../src/TcpClient.h"
#include <ncurses.h>
#include <unistd.h>
#include <sstream>
#include <errno.h>
#include <vector>
#include <string>

WINDOW* window = NULL;
void setupWindow() {
	window = initscr();
	if(nodelay(window, true) == ERR) {
		printw("nodelay init failed.\n");
		exit(1);
	}
}

class NoInputException : public std::exception {};
char getInput() {
	char in = wgetch(window);
	if (in == ERR)
		throw NoInputException();
	return in;
}

int main() {
	setupWindow();
	printw("Connecting...\n");
	while(true) {
		try {
			jsock::TcpClient socket("127.0.0.1", 1234);
			printw("Connected!\n");
			while(true) {
				std::stringstream stream;
				char in = 0;
				printw("> ");
				do {
					try { stream << (in = getInput()); }
					catch(const NoInputException& e) {}
					std::vector<unsigned char> data = socket.read();
					if (data.size() > 0) {
						printw("\r%s", std::string(&data[0],
							&data[0] + data.size()).c_str());
						printw("> %s", stream.str().c_str()); 
					}
				} while(in != '\n');
				const std::string& str = stream.str();
				printw("> %s", str.c_str());
				socket.write(std::vector<unsigned char>(&str[0], 
					&str[0] + str.size()));
			}
		}
		catch(const jsock::SocketException& problem) {
			printw("\r"); // reset current line
			switch(problem.errorNumber()) {
				case EPIPE:
					printw("Disconnected!\n");
					break;
				case ECONNREFUSED:
					break;
				default:
					printw("Error %d\n", problem.errorNumber());
			}

			// make sure to refresh now because we're probably
			// gonna block when the socket is constructed
	 		wrefresh(window);
		}
	}
}

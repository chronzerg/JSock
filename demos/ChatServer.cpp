#include "../src/TcpServer.h"
#include <algorithm>
#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <errno.h>
#include <vector>
#include <string>

WINDOW* setupWindow() {
	WINDOW* window = initscr();
	if(nodelay(window, true) == ERR) {
		printw("nodelay init failed");
		exit(1);
	}
	return window;
}

// Collect input from stdin. Send what we've collected
// when we reach a newline.
void processSocketOutput(const jsock::TcpEndpoint& socket,
		std::stringstream& stream) {
	char in = getch();
	if (in != ERR) {
		stream << in;
		if (in == '\n') {
			const std::string& str = stream.str();
			printw("> %s", str.c_str());
			socket.write(std::vector<unsigned char>(&str[0],
				&str[0] + str.size()));

			// Reset the stream
			stream.str("");
			stream.clear();
			printw("> ");
		}
	}
}

// Check for socket input. If we have some, print it do
// the screen.
void processSocketInput(const jsock::TcpEndpoint& socket,
		std::stringstream& stream) {
	static std::vector<unsigned char> data;

	// Read from the socket and append it to our static vector.
	std::vector<unsigned char> received = socket.read();
	data.insert(data.end(), received.begin(), received.end());

	// Find the last instance of the newline character.
	std::vector<unsigned char>::reverse_iterator lastNewline;
	lastNewline = std::find(data.rbegin(), data.rend(), '\n');

	// If we found something, grab everything before the newline
	// and print it. Leave the rest in the static vector.
	if (lastNewline != data.rend()) {
		std::vector<unsigned char> message(data.begin(),
			lastNewline.base());
		data.erase(data.begin(), lastNewline.base());

		message.push_back('\0');
		printw("\r%s", &message[0]);

		// Redraw the current line of input after the message we
		// just received.
		printw("> %s", stream.str().c_str());
	}
}

int main(int argCount, char* argArray[]) {
	WINDOW* window = setupWindow();
	printw("Connecting...\n");
	while(1) {
		try {
			jsock::TcpServer server(1234);
			jsock::TcpEndpoint socket = server.accept();
			printw("Connected!\n");
			std::stringstream stream;
			printw("> ");
			while(1) {
				processSocketOutput(socket, stream);
				processSocketInput(socket, stream);
			}
		}
		catch(const jsock::SocketException& problem) {
			printw("\r"); // reset current line
			switch(problem.errorNumber()) {
				case EPIPE:
					printw("Disconnected!\n");
					break;
				case ECONNREFUSED:
					// ignore
					break;
				default:
					endwin();
					std::cout
						<< "Error "
						<< problem.errorNumber()
						<< std::endl;
					exit(1);
			}

			// Make sure to refresh now because we're probably
			// gonna block when the socket is constructed.
	 		wrefresh(window);
		}
	}
}

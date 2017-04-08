#include "../src/TcpServer.h"
#include <algorithm>
#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <errno.h>
#include <memory>
#include <vector>
#include <string>

// Collects message fragments from the underlying
// endpoint and provides them to the caller as a
// completed message.
class ParsingEndpoint: public virtual jsock::Endpoint {
	private:
		std::unique_ptr<jsock::Endpoint> socket;
		std::unique_ptr<std::vector<unsigned char>> buffer;

	public:
		ParsingEndpoint(std::unique_ptr<jsock::Endpoint> socket):
				socket(std::move(socket)),
				buffer(new std::vector<unsigned char>()) {}

		// Passthrough to the underlying socket
		void write(const std::vector<unsigned char>& data) const {
			this->socket->write(data);
		}

		// Reads data from the socket and appends it to the running
		// data buffer. If at least one message is completed, return
		// it/them to the caller. Otherwise, return an empty vector.
		std::vector<unsigned char> read() const {
			std::vector<unsigned char> received = this->socket->read();
			this->buffer->insert(this->buffer->end(), received.begin(),
					received.end());

			// Find the last instance of the newline character. This
			// signifies a message boundary.
			std::vector<unsigned char>::reverse_iterator lastNewline;
			lastNewline = std::find(buffer->rbegin(), this->buffer->rend(), '\n');

			// If we found something, grab everything before the newline
			// and print it. Leave the rest in buffer.
			if (lastNewline != buffer->rend()) {
				std::vector<unsigned char> message(this->buffer->begin(),
					lastNewline.base());
				this->buffer->erase(this->buffer->begin(), lastNewline.base());
				message.push_back('\0');
				return message;
			}
			return std::vector<unsigned char>();
		}

		jsock::Name getName(Side side) const {
			return this->socket->getName(side);
		}
};

// Initializes the ncurses library as our UI
// framework.
WINDOW* setupWindow() {
	WINDOW* window = initscr();
	if(nodelay(window, true) == ERR) {
		printw("nodelay init failed");
		exit(1);
	}
	return window;
}

// Attempt to accept a new connection from the TCP server.
// If we get one, add it to the endpoints vector.
void acceptNewConnections(
		std::vector<std::unique_ptr<jsock::Endpoint>>& endpoints,
		std::stringstream& stream,
		jsock::TcpServer& server) {
	std::unique_ptr<jsock::Endpoint> client = server.accept();
	if (client) {
		std::string remote = client->getName(jsock::Endpoint::Remote);
		std::string local = client->getName(jsock::Endpoint::Local);
		printw("\rConnected to %s via %s\n",
				remote.c_str(), local.c_str());
		printw("> %s", stream.str().c_str());
		endpoints.push_back(
			std::unique_ptr<jsock::Endpoint>(
				new ParsingEndpoint(std::move(client))));
	}
}

// Collect input from stdin. Send what we've collected
// when we reach a newline.
void processSocketOutput(
		const std::vector<std::unique_ptr<jsock::Endpoint>>& endpoints,
		std::stringstream& stream) {
	char in = getch();
	if (in != ERR) {
		stream << in;
		if (in == '\n') {
			const std::string& str = stream.str();
			printw("> %s", str.c_str());
			for(const auto& socket: endpoints) {
				socket->write(std::vector<unsigned char>(&str[0],
					&str[0] + str.size()));
			}

			// Reset the stream
			stream.str("");
			stream.clear();
			printw("> ");
		}
	}
}

// Check the endpoints for new messages. If there are any, print
// them to the screen and resend them to the other clients.
void processSocketInput(
		std::vector<std::unique_ptr<jsock::Endpoint>>& endpoints,
		std::stringstream& stream) {
	bool output = false;
	for(const auto& recvpoint: endpoints) {
		std::vector<unsigned char> message = recvpoint->read();
		if (message.size() > 0) {
			printw("\r%s", &message[0]);
			for(const auto& sendpoint: endpoints)
				if(sendpoint != recvpoint)
					sendpoint->write(message);
			output = true;
		}
	}
	// If we printed out some input, redraw the current line of
	// input.
	if (output) printw("> %s", stream.str().c_str());
}

int main() {
	WINDOW* window = setupWindow();
	while(true) {
		try {
			jsock::TcpServer server(1234);
			printw("Server listening\n");
			std::vector<std::unique_ptr<jsock::Endpoint>> endpoints;
			std::stringstream stream;
			printw("> ");
			while(true) {
				acceptNewConnections(endpoints, stream, server);
				processSocketOutput(endpoints, stream);
				processSocketInput(endpoints, stream);
			}
		}
		catch(const jsock::SocketException& problem) {
			printw("\r"); // reset current line
			switch(problem.errorNumber()) {
				case EPIPE:
				case ECONNRESET:
					printw("Disconnected!\n");
					break;
				case ECONNREFUSED:
					// ignore
					break;
				default:
					endwin(); // shutdown ui framework
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

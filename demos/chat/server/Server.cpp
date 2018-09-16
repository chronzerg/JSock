#include "ParsingEndpoint.h"
#include "jsock/TcpServer.h"
#include <algorithm>
#include <errno.h>
#include <iostream>
#include <memory>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

// Initializes the ncurses library as our UI
// framework.
WINDOW *setupWindow() {
  WINDOW *window = initscr();
  if (nodelay(window, true) == ERR) {
    printw("nodelay init failed");
    exit(1);
  }
  return window;
}

// Attempt to accept a new connection from the TCP server.
// If we get one, add it to the endpoints vector.
void acceptNewConnections(
    std::vector<std::unique_ptr<jsock::Endpoint>> &endpoints,
    std::stringstream &stream, jsock::TcpServer &server) {
  std::unique_ptr<jsock::Endpoint> client = server.accept();
  if (client) {
    std::string remote = client->getRemoteName();
    std::string local = client->getLocalName();
    printw("\rConnected to %s via %s\n", remote.c_str(), local.c_str());
    printw("> %s", stream.str().c_str());
    endpoints.push_back(std::unique_ptr<jsock::Endpoint>(
        new ParsingEndpoint(std::move(client))));
  }
}

// Collect input from stdin. Send what we've collected
// when we reach a newline.
void processSocketOutput(
    const std::vector<std::unique_ptr<jsock::Endpoint>> &endpoints,
    std::stringstream &stream) {
  char in = getch();
  if (in != ERR) {
    stream << in;
    if (in == '\n') {
      const std::string &str = stream.str();
      printw("> %s", str.c_str());
      for (const auto &socket : endpoints) {
        socket->write(
            std::vector<unsigned char>(&str[0], &str[0] + str.size()));
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
    std::vector<std::unique_ptr<jsock::Endpoint>> &endpoints,
    std::stringstream &stream) {
  bool output = false;
  for (const auto &recvpoint : endpoints) {
    std::vector<unsigned char> message = recvpoint->read();
    if (message.size() > 0) {
      printw("\r%s", &message[0]);
      for (const auto &sendpoint : endpoints)
        if (sendpoint != recvpoint)
          sendpoint->write(message);
      output = true;
    }
  }
  // If we printed out some input, redraw the current line of
  // input.
  if (output)
    printw("> %s", stream.str().c_str());
}

int main() {
  WINDOW *window = setupWindow();
  while (true) {
    try {
      jsock::TcpServer server(1234);
      printw("Server listening\n");
      std::vector<std::unique_ptr<jsock::Endpoint>> endpoints;
      std::stringstream stream;
      printw("> ");
      while (true) {
        acceptNewConnections(endpoints, stream, server);
        processSocketOutput(endpoints, stream);
        processSocketInput(endpoints, stream);
      }
    } catch (const jsock::SocketException &problem) {
      printw("\r"); // reset current line
      switch (problem.errorNumber) {
      case EPIPE:
      case ECONNRESET:
        printw("Disconnected!\n");
        break;
      case ECONNREFUSED:
        // ignore
        break;
      default:
        endwin(); // shutdown ui framework
        std::cout << "Error " << problem.errorNumber << std::endl;
        exit(1);
      }

      // Make sure to refresh now because we're probably
      // gonna block when the socket is constructed.
      wrefresh(window);
    }
  }
}

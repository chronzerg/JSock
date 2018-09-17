#include "jsock/TcpEndpoint.h"
#include <algorithm>
#include <errno.h>
#include <iostream>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

WINDOW *setupWindow() {
  WINDOW *window = initscr();

  // Configure getch() to wait for up to 100ms for input.
  if (halfdelay(1) == ERR) {
    exit(1);
  }

  // Configure keypresses to not automatically echo to the screen.
  if (noecho() == ERR) {
    exit(2);
  }

  // Configure the window to automatically scroll.
  if (scrollok(window, true) == ERR) {
    exit(3);
  }

  return window;
}

// Collect input from stdin. Send what we've collected when we reach a newline.
void processSocketOutput(jsock::TcpEndpoint &socket,
                         std::ostringstream &stream) {
  // Read input from user. If there isn't any input available, return.
  char in = getch();
  if (in == ERR) {
    return;
  }

  // Add input into stream, unless it's a backspace character... in that case
  // remove the last character we added.
  if (in == 127) {
    std::string str = stream.str();
    if (str.size() > 0) {
      str.erase(str.size() - 1, 1);
      stream.str(str);
    }
  } else {
    stream << in;
  }

  // Print to screen what we have so far.
  std::string str = stream.str();
  printw("\r> %s", str.c_str());
  clrtoeol();

  // If the last character entered was a newline, send that line of text to the
  // server and reset our stream.
  if (in == '\n') {
    socket.write(std::vector<unsigned char>(&str[0], &str[0] + str.size()));
    stream.str("");
    stream.clear();
    printw("> ");
  }
}

// Check for socket input. If we have some, print it to the screen.
void processSocketInput(jsock::TcpEndpoint &socket,
                        std::ostringstream &stream) {
  static std::vector<unsigned char> data;

  // Read from the socket and append it to our static vector.
  std::vector<unsigned char> received = socket.read();
  data.insert(data.end(), received.begin(), received.end());

  // Find the last instance of the newline character.
  std::vector<unsigned char>::reverse_iterator lastNewline;
  lastNewline = std::find(data.rbegin(), data.rend(), '\n');

  // If we found something, grab everything before the newline and print it.
  // Leave the rest in the static vector.
  if (lastNewline != data.rend()) {
    std::vector<unsigned char> message(data.begin(), lastNewline.base());
    data.erase(data.begin(), lastNewline.base());

    message.push_back('\0');
    printw("\r%s", &message[0]);

    // Redraw the current line of input after the message we just received.
    std::string str = stream.str();
    printw("> %s", str.c_str());
  }
}

int main() {
  WINDOW *window = setupWindow();

  printw("Connecting...\n");
  while (true) {
    try {
      // Attempt to connect to the server. If this fails, a SocketException is
      // thrown and we jump down to the catch block below.
      jsock::TcpEndpoint socket(jsock::Name("127.0.0.1", 1234));

      // If the connection suceeds, print out a summary about it.
      std::string remote = socket.getRemoteName();
      std::string local = socket.getLocalName();
      printw("Connected to %s via %s\n", remote.c_str(), local.c_str());

      // Setup our out-stream in append-mode, meaning any writing is always done
      // onto the end of the stream. This stream holds the current line of user
      // input before enter is pressed.
      std::ostringstream stream(std::ios_base::app);

      printw("> ");
      while (true) {
        processSocketOutput(socket, stream);
        processSocketInput(socket, stream);
      }
    } catch (const jsock::SocketException &problem) {
      switch (problem.errorNumber) {
      case EPIPE:
      case ECONNRESET:
        printw("\rDisconnected!\n");
        break;
      case ECONNREFUSED:
        // This error occurs if there isn't a TCP server listening for our
        // connection. We will ignore this and keep trying to connect.
        break;
      default:
        printw("\rError %d\n", problem.errorNumber);
      }

      // Make sure to refresh now because we're probably gonna block when the
      // socket is constructed.
      wrefresh(window);
    }
  }
}

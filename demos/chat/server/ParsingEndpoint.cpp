#include "ParsingEndpoint.h"

ParsingEndpoint::ParsingEndpoint(std::unique_ptr<jsock::Endpoint> socket):
		socket(std::move(socket)),
		buffer(new std::vector<unsigned char>()) {}

// Passthrough to the underlying socket
void ParsingEndpoint::write(const std::vector<unsigned char>& data) const {
	this->socket->write(data);
}

// Reads data from the socket and appends it to the running
// data buffer. If at least one message is completed, return
// it/them to the caller. Otherwise, return an empty vector.
std::vector<unsigned char> ParsingEndpoint::read() const {
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

jsock::Name ParsingEndpoint::getName(Side side) const {
	return this->socket->getName(side);
}

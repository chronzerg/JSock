#ifndef StreamSocket_h
#define StreamSocket_h

namespace raiisocket {

class StreamSocket {
	private:
		int fileDescriptor;

	public:
		StreamSocket();
		~StreamSocket();
		operator int();
};

} // namespace RaiiSocket

#endif // StreamSocket_h

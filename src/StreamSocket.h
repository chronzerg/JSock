#ifndef StreamSocket_h
#define StreamSocket_h

namespace jsock {

class StreamSocket {
	private:
		int fileDescriptor;

	public:
		StreamSocket();
		~StreamSocket();
		operator int();
		void throwIfError();
};

} // namespace jsock

#endif // StreamSocket_h

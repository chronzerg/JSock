#ifndef StreamSocket_h
#define StreamSocket_h

namespace jsock {

class StreamSocket {
	private:
		int fileDescriptor;

	public:
		StreamSocket();
		StreamSocket(unsigned short fileDescriptor);
		~StreamSocket();
		operator int() const;
		void throwIfError() const;
};

} // namespace jsock

#endif // StreamSocket_h

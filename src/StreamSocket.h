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

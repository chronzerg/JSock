#ifndef Endpoint_h
#define Endpoint_h

#include "Name.h"
#include <string>
#include <vector>

namespace jsock {

class Endpoint {
	public:
		enum Side { Local, Remote };
		virtual void write(const std::vector<unsigned char>&) const = 0;
		virtual std::vector<unsigned char> read() const = 0;
		virtual Name getName(Side side) const = 0;
};

} // namespace jsock

#endif // Endpoint_h

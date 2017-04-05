#ifndef Endpoint_h
#define Endpoint_h

#include "Authority.h"
#include <string>
#include <vector>

namespace jsock {

class Endpoint {
	public:
		virtual void write(const std::vector<unsigned char>&) const = 0;
		virtual std::vector<unsigned char> read() const = 0;
		virtual Authority remote() const = 0;
		virtual Authority local() const = 0;
};

} // namespace jsock

#endif // Endpoint_h

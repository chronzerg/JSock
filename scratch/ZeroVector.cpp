#include <iostream>
#include <vector>

int main() {
	unsigned char byte;
	std::vector<unsigned char> v(&byte, &byte-1);	
	std::cout << v.size() << std::endl;
}

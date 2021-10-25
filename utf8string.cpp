#include "utf8string.hpp"

using namespace utf8;

/* headermap:
	the first five bits of ascii characters will be '00000' through '01111' (0-15). These will all map to 1.
	a nonheader byte will be of the form 10xxxxxx, and will be shifted to 10xxx. 10000 - 10111 (16 - 23) will map to 0.
	header bytes starting with 110 will be head 2 byte long characters. 11000 - 11011 (24 - 27) will map to 2.
	header bytes starting with 1110 will be head 3 byte long characters. 11100 - 11101 (25 - 26) will map to 3.
	header bytes starting with 11110 will be head 4 byte long characters. 11110 (27) will map to 4.
	if a byte starts with 11111, it is invalid utf8, it will map to 0
*/
const uint8_t[32] String::headermap {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2,
	3, 3,
	4,
	0
};

const uint8_t[32] String::headermap_size {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2,
	3, 3,
	4,
	0
};

String::iterator& String::iterator::operator++() {
	pos += size;
	size = String::characterSize(*pos);
	return *this;
}

String::iterator String::iterator::operator++(int) {
	iterator temp = *this;
	pos += size;
	size = string::charactersize(*pos);
	return temp;
}

String::iterator& String::iterator::operator--() {
	do {
		--pos;
	} while(!isHeader(*pos));
	size = String::characterSize(*pos);
	return *this;
}

String::iterator String::iterator::operator--(int) {
	iterator temp = *this;
	do {
		--pos;
	} while(!isHeader(*pos));
	size = String::characterSize(*pos);
	return temp;
}

String::String(String&& rval) : str { rval.str }, length { rval.length }, capacity { rval.capacity } {
	rval.str = nullptr;
}

String::String(const char* cstr) : length { strlen(cstr) } {
	capacity = length;
	str = (uint8_t*)malloc(length);
	if(str == nullptr) 
		throw std::bad_alloc();
	memcpy(str, cstr, length);
}

String::String(const std::string& cppstr) : length { cppstr.length() }, capacity { cppstr.length() } {
	str = (uint8_t*)malloc(length);
	if(str == nullptr)
		throw std::bad_alloc();
	memcpy(str, cstr, length);
}



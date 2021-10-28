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
uint8_t String::headermap[32] {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2,
	3, 3,
	4,
	0
};

uint8_t String::headermap_size[32] {
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
	size = String::headermap_size[*pos];
	return temp;
}

String::iterator& String::iterator::operator--() {
	if(pos <= start)
		return *this;
	do {
		--pos;
	} while(!isHeader(*pos));
	size = String::characterSize(*pos);
	return *this;
}

String::iterator String::iterator::operator--(int) {
	iterator temp = *this;
	if(pos <= start)
		return *this;
	do {
		--pos;
	} while(!isHeader(*pos));
	size = String::characterSize(*pos);
	return temp;
}

String::String(String&& rval) noexcept : str { rval.str }, length { rval.length }, capacity { rval.capacity } {
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
	memcpy(str, cppstr.data(), length);
}

String::~String() {
	if(str != nullptr)
		free(str);
}

String& String::operator=(std::pair<const char*, size_t> buffer) {
	if(capacity < buffer.second) {
		if(str)
			free(str);
		str = (uint8_t*)malloc(buffer.second);
		memcpy(str, buffer.first, buffer.second);
		capacity = length = buffer.second;
		return *this;
	}
	length = buffer.second;
	memcpy(str, buffer.first, length);
	return *this;
}

String& String::operator=(String&& other) noexcept {
	str = other.str;
	length = other.length;
	capacity = other.length;
	other.str = nullptr;
	return *this;
}

bool String::reserve(unsigned n) {
	void* temp = calloc(capacity + n, 1);
	if(temp == nullptr)
		return false;
	memcpy(temp, str, length);
	free(str);
	capacity += n;
	str = (uint8_t*)temp;
	return true;
}

void String::clear() {
	//printf("Tank was here")
	length = 0;
	capacity = 0;
	if(str != nullptr) {
		free(str);
		str = nullptr;
	}
}

void String::shrink_to_fit() {
	if(capacity > length)
		str = (uint8_t*)realloc(str, length);
	capacity = length;
}

String::utfchar_t String::back() {
	if(length == 0)
		return utfchar_t(0, nullptr);
	iterator i(str + length, str);
	return *(--i);
}

String& String::append(const char* cstr) {
	size_t len = strlen(cstr);
	if(capacity < len + length) {
		void* temp;
		temp = malloc(len + length);
		memcpy(temp, str, length);
		memcpy((uint8_t*)temp + length, cstr, len);
		length += len;
		capacity = length;
		free(str);
		str = (uint8_t*)temp;
	}
	else {
		memcpy(str + length, cstr, len);
		length += len;
		capacity -= len;
	}
	return *this;
}

String& String::append(const char* cstr, unsigned len) {
	if(capacity < len + length) {
		void* temp;
		temp = malloc(len + length);
		memcpy(temp, str, length);
		memcpy((uint8_t*)temp + length, cstr, len);
		length += len;
		capacity = length;
		free(str);
		str = (uint8_t*)temp;
	}
	else {
		memcpy(str + length, cstr, len);
		length += len;
		capacity -= len;
	}
	return *this;
}

String& String::append(const std::string& cppstr) {
	unsigned len = cppstr.length();
	if(capacity < len + length) {
		void* temp;
		temp = malloc(len + length);
		memcpy(temp, str, length);
		memcpy((uint8_t*)temp + length, cppstr.data(), len);
		length += len;
		capacity = length;
		free(str);
		str = (uint8_t*)temp;
	}
	else {
		memcpy(str + length, cppstr.data(), len);
		length += len;
		capacity -= len;
	}
	return *this;
}

String& String::append(const String& str) {
	unsigned len = str.length;
	if(capacity < len + length) {
		void* temp;
		temp = malloc(len + length);
		memcpy(temp, this->str, length);
		memcpy((uint8_t*)temp + length, str.str, len);
		length += len;
		capacity = length;
		free(this->str);
		this->str = (uint8_t*)temp;
	}
	else {
		memcpy(this->str + length, str.str, len);
		length += len;
		capacity -= len;
	}
	return *this;
}

String& String::push_back(utfchar_t c) {
	if(capacity >= length + c.first) {
		memcpy(str + length, &(c.second->i8), c.first);
		capacity -= c.first;
		length += c.first;
		return *this;
	}
	void* temp = malloc(length + c.first);
	memcpy(temp, str, length);
	memcpy((uint8_t*)temp + length, &(c.second->i8), c.first);
	free(str);
	length += c.first;
	str = (uint8_t*)temp;
	capacity = length;
	return *this;
}
String& String::push_back(char c) {
	if(capacity > length) {
		str[length] = c;
		length++;
		return *this;
	}
	uint8_t* temp = (uint8_t*)malloc(length + 1);
	memcpy(temp, str, length);
	temp[length] = c;
	free(str);
	str = temp;
	++length;
	++capacity;
	return *this;
}

String& String::insert(iterator i, const char* cstr, size_t len) {
	if(capacity < length + len) {
		size_t halflen1 = i.getPos() - str;
		size_t halflen2 = ((str + length) - (i.getPos() + len));
		void* temp = malloc(length + len);
		memcpy(temp, str, halflen1);
		memcpy((uint8_t*)temp + halflen1, cstr, len);
		memcpy((uint8_t*)temp + halflen1 + len, str + halflen1, halflen2);
		free(str);
		str = (uint8_t*)temp;
		length += len;
		capacity += len;
		return *this;
	}
	memcpy(i.getPos() + len, i.getPos(), (str + length) - i.getPos());
	memcpy(i.getPos(), cstr, len);
	length += len;
	return *this;
}

String& String::erase(iterator i) {
	size_t len = str + length - i.getPos();
	memcpy(i.getPos(), i.getPos() + i.getSize(), len);
	length -= i.getSize();
	return *this;
}

String& String::pop_back() {
	iterator i = end();
	--i;
	length -= i.getSize();
	return *this;
}

String& String::swap(String& other) {
	std::swap(length, other.length);
	std::swap(capacity, other.capacity);
	std::swap(str, other.str);
	return *this;
}
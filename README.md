# utf8-string-cpp
A simple implementation of utf8 strings for C++

DOCS:
How to include this project:
- include utf8string.hpp in whatever files need it.
- add utf8string.cpp to your source code

Data Types:
- utf8::String::utfchar_t - alias for std::pair<uint8_t, Character*> (NOTE: utf8::String::Character will be defined later)
- utf8::String::iterator
- - utf::String::iterator::iterator(uint8_t* pos, uint8_t* start): constructs an iterator starting at pos which cannot be decremented below start
- - operator++: increments iterator to next utf8 character (may increment by more than a byte)
- - operator--: decrements iterator to previous utf8 character, will not decrement past start (second arg in constructor)
- - operator*: returns a utfchar_t with the first value being the size of the character and the second value pointing to the character that the iterator points to
- - getSize(): returns the size of the character that the iterator points to
- - getPos(): returns the location at which the iterator points
- - setPos(uint8_t* new_pos): sets the position of the iterator to new_pos. The size will be adjusted. WARNING: if new_pos is below start, decrementing will not be possible
- utf8::String::Character
- - NOTE: &Character = &i8 = i16 = i24 = i32
- - i8: first value
- - i16: first two bytes sequentially (unnaffected by endianess)
- - i24: first three bytes sequentially
- - i32: first four bytes sequentially
- utf8::String
- - isHeader(uint8_t): for valid utf8 character headers, returns the size of said header. Otherwise, returns 0 (continuation bytes are not headers)
- - characterSize(uint8_t): For utf8 character headers, returns the size of said header, otherwise, returns 1
- - String() noexcept: construct an empty string (length and capacity 0)
- - String(String&&) noexcept: move constructor
- - String(const char*): construct from c string (NOT SAFE IF NO NULL TERMINATOR). Throws std::bad_alloc if unable to allocate memory
- - String(const String&): construct from string. Throws std::bad_alloc if unable to allocate memory
- - operator=(std::pair<const char*, size_t>): assign from buffer (whose size is the second element of the pair)
- - operator=(const std::string&): assign from string
- - operator=(const utf8::String&): assign from utf8 string
- - operator=(utf8::String&&): move from utf8 string
- - begin(): return iterator to begining;
- - end(): return iterator pointing to byte after end of string. String is bounded between \[begin, end)
- - cdata(): return constant pointer to array stored by string
- - data(): return pointer to array stored by string
- - getLength(): return size of string (amount of bytes)
- - getSize(): same as above
- - getCapacity(): return capacity of string (amount of bytes)
- - bool empty(): return whether or not the string is empty (returns true for empty strings)
- - bool reserve(unsigned): request a change in capacity. Returns true if able to allocate. WARNING: runs in O(n) time, since it copies all data to a larger buffer
- - void clear(): clear the string (capacity and length are set to zero, data is nullptr).
- - void shrink_to_fit(): if capacity is greater than length, deallocate unused bytes. 
- - front(): return a utfchar_t pointing to the front of the string. WARNING: Segmentation Faults for empty strings
- - back(): return a utfchar_t pointing to the final character of the string. NOTE: safe for empty strings
- - append(const char*): append cstring. WARNING: not safe if cstring has no null terminator
- - append(const char*, unsigned): append buffer
- - append(const std::string&): append string
- - append(const utf8::String&): append utf8 string
- - push_back(utfchar_t): push back character
- - push_back(uint8_t, uint32_t): push back a utf8 character of size equal to the first argument
- - push_back(char): push back character
- - insert(iterator, [args]): insert arg at iterator
- - erase(iterator): erase character at iterator
- - swap(String&): swap contents

Lone Functions:
- opterator<<(std::ostream&, utf8::String): insert string to output stream

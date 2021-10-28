#pragma once
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <iostream>

namespace utf8 {
	class String {
		private:
		static uint8_t headermap[32]; //maps first five bits of a character to its header status (is it a header? if so, how big is the character)
		static uint8_t headermap_size[32]; //same as headermap, but continuation headers map to 1 (10xxx)

		uint8_t* str;
		size_t length;
		size_t capacity;

		public:
		union Character {
			uint8_t i8;
			uint8_t i16[2];
			uint8_t i24[3];
			uint8_t i32[4];
		};
		class iterator {
			friend class String;
			public:
			iterator(uint8_t* pos, uint8_t* start) : pos { pos }, size { String::characterSize(*pos) },
			start { start } {}
			inline uint8_t* getPos() const { return pos; }
			inline uint8_t getSize() const { return size; }
			inline void setPos(void* new_pos) { 
				pos = (uint8_t*)new_pos; 
				size = String::characterSize(*pos);
			}

			iterator& operator++();
			iterator operator++(int);
			//WARNING: decrement is unsafe if the string is not valid utf8!
			iterator& operator--();
			iterator operator--(int);

			//first value is size of character
			inline std::pair<uint8_t, String::Character*> operator*() {
				return std::pair<uint8_t, String::Character*>(size, (String::Character*)pos);
			}

			private:
			uint8_t* pos;
			uint8_t* start;
			uint8_t size;
		};
		using utfchar_t = std::pair<uint8_t, Character*>; //first element is size in bytes
			
		//a header is the start of a utf character. NOTE: all ascii characters will be considered headers of one byte long characters
		//isHeader returns zero if it's not a header, and the size of the character that it heads if it is a header
		static inline uint8_t isHeader(uint8_t b) {
			return headermap[b >> 3];
		}
		//returns 1 for continuation bytes
		static inline uint8_t characterSize(uint8_t b) {
			return headermap_size[b >> 3];
		}

		String() noexcept : str { nullptr }, length { 0 }, capacity { 0 } {}
		String(String&&) noexcept;
		String(const char*); //NOTE: assumes that there is a null terminator
		String(const std::string&);

		~String();

		String& operator=(std::pair<const char*, size_t>);
		inline String& operator=(const char* cstr) {
			return *this = std::pair<const char*, size_t>(cstr, strlen(cstr));
		}
		inline String& operator=(const std::string& cppstr) {
			return *this = std::pair<const char*, size_t>(cppstr.data(), cppstr.length());
		}
		inline String& operator=(const String& utf8str) {
			return *this = std::pair<const char*, size_t>((char*)utf8str.str, utf8str.length);
		}
		String& operator=(String&& other) noexcept;

		iterator begin() { return iterator(str, str); }
		iterator end() { return iterator(str + length, str); }
		const uint8_t* cdata() const { return str; }
		uint8_t* data() { return str; }
		
		unsigned getLength() const { return length; }
		unsigned getSize() const { return length; }
		unsigned getCapacity() const { return capacity; }
		bool empty() const { return length == 0; }

		bool reserve(unsigned n); //returns 1 if able to reserve, else 0
		void clear();
		void shrink_to_fit();

		utfchar_t front() { return utfchar_t(headermap_size[*str], (Character*)str); }
		utfchar_t back(); 

		String& append(const char*);
		String& append(const char*, unsigned);
		String& append(const std::string&);
		String& append(const String&);
		String& push_back(utfchar_t);
		String& push_back(uint8_t size, uint8_t character) {
			return push_back(utfchar_t(size, (Character*)&character));
		}
		String& push_back(char);

		//inserts before iterator. NOTE: iterator must be a valid iterator, otherwise, undefined
		String& insert(iterator, const char*, size_t);
		inline String& insert(iterator i, utfchar_t c) {
			return insert(i, (char*)&(c.second->i8), c.first);
		}
		inline String& insert(iterator i, const char* cstr)  {
			size_t len = strlen(cstr);
			return insert(i, cstr, len);
		}
		inline String& insert(iterator i, const std::string cppstr) {
			return insert(i, cppstr.data(), cppstr.length());
		}
		inline String& insert(iterator i, const String& utf8str) {
			return insert(i, (char*)utf8str.str, utf8str.length);
		}

		//NOTE: the following functions decrease length, but not capacity
		String& erase(iterator);
		String& pop_back();

		String& swap(String& other);
	};
}

inline std::ostream& operator<<(std::ostream& os, const utf8::String& s) {
	os.write((const char*)s.cdata(), s.getSize());
	return os;
}
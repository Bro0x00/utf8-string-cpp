#pragma once
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdint>
#include <utility>
#include <stdexcept>

namespace utf8 {
	class String {
		public:
		union Character {
			uint8_t i8;
			uint8_t[2] i16;
			uint8_t[3] i24;
			uint8_t[4] i32;
		};
		class iterator {
			public:
			iterator(uint8_t* pos) : pos { pos }, size { String::characterSize(*pos) } {}
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
				return std::pair<uint8_t, String::Character&>(size, (String::Character*)pos);
			}

			private:
			uint8_t* pos;
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

		iterator begin() { return iterator(str); }
		iterator end() { return iterator(str + length); }
		const uint8_t* cdata() const { return str; }
		uint8_t* data() { return str; }
		
		unsigned length() const { return length; }
		unsigned size() const { return length; }
		unsigned capacity() const { return capacity; }
		bool empty() const { return size == 0; }

		bool reserve(unsigned n); //returns 1 if able to reserve, else 0
		void clear();
		void shrink_to_fit();

		utfchar_t front() { return utfchar_t(characterSize[*str], str); }
		utfchar_t back(); 

		String& append(const char*);
		String& append(const std::string&);
		String& append(const String&);
		String& push_back(utfchar_t);

		String& insert(iterator, utfchar_t);
		String& insert(iterator, const char*);
		String& insert(iterator, const std::string);
		String& insert(iterator, const String&);

		String& erase(iterator);
		String& pop_back();

		String& swap(String& other);

		private:
		static const uint8_t[32] headermap; //maps first five bits of a character to its header status (is it a header? if so, how big is the character)
		static const uint8_t[32] headermap_size; //same as headermap, but continuation headers map to 1 (10xxx)

		uint8_t* str;
		unsigned length;
		unsigned capacity;
	}

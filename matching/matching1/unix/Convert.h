#pragma once
#include <vector>
#include <basic/tstring.h>
#include <basic/TextConv.h>

inline const char *convertToCharIfNecessary(const char *chars, \
											std::vector<char> &ignored) {

	return chars;
}

inline const char *convertToUTF(const char *value, \
								std::vector<char> &buffer) {

       assert(value);
       buffer.resize(cp1251_to_utf8_len(value) + 1);
       return cp1251_to_utf8(value, &buffer[0]);
}

inline void copyImmutableStrToVector(ImmutableString<char> str, std::vector<char> &dest) {

	assert(!str.isNull());
	dest.resize(str.size + 1);

	strncpy(&dest[0], str.str, str.size);
	dest[str.size] = '\0';
}
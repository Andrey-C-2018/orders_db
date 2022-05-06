#pragma once
#include <vector>
#include <basic/tstring.h>
#include <basic/TextConv.h>

inline const char *convertIfNecessary(const char *text, std::string &buffer) {

       assert(text);
       buffer.resize(cp1251_to_utf8_len(text));
       return cp1251_to_utf8(text, &buffer[0]);
}

inline const char *convertIfNecessary(const char *text, std::vector<char> &buffer) {

       assert(text);
       buffer.resize(cp1251_to_utf8_len(text) + 1);
       return cp1251_to_utf8(text, &buffer[0]);
}

inline const char *convertSimple(const char *value, std::vector<char> &buffer) {

	return value;
}
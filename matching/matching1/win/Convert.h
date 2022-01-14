#pragma once
#include <string>
#include <vector>

const char *convertToCharIfNecessary(const wchar_t *widechars, std::vector<char> &chars) {

	assert(widechars);
	size_t len = wcslen(widechars);
	chars.resize(len + 1);
	wcstombs(&chars[0], widechars, len);
	chars[len] = '\0';
	return &chars[0];
}

inline const char *convertFromCP1251IfNecessary(const char *value, std::string &buffer) {

       buffer = value;
       return buffer.c_str();
}

inline const char *convertFromCP1251IfNecessary(const char *value, std::vector<char> &buffer) {

       size_t value_size = strlen(value);
       buffer.resize(value_size + 1);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
       strncpy(&buffer[0], value, value_size);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
       buffer[value_size] = '\0';
       return &buffer[0];
}
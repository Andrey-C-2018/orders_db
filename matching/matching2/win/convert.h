#pragma once
#include <string>
#include <vector>

inline const char *convertIfNecessary(const wchar_t *value, std::string &buffer) {

	size_t value_size = wcslen(value);
	buffer.resize(value_size);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
	wcstombs(&buffer[0], value, value_size);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	return buffer.c_str();
}

inline const char *convertIfNecessary(const wchar_t *value, std::vector<char> &buffer) {

	size_t value_size = wcslen(value);
	buffer.resize(value_size + 1);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
	wcstombs(&buffer[0], value, value_size);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	buffer[value_size] = '\0';
	return &buffer[0];
}

inline const wchar_t *convertSimple(const char *value, std::vector<wchar_t> &buffer) {

	size_t value_size = strlen(value);
	buffer.resize(value_size + 1);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
	mbstowcs(&buffer[0], value, value_size);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

	buffer[value_size] = L'\0';
	return &buffer[0];
}
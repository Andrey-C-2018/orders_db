#pragma once
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

inline const char *convertToCharIfNecessary(const wchar_t *widechars, \
											std::vector<char> &chars) {

	assert(widechars);
	size_t len = wcslen(widechars);
	chars.resize(len + 1);

	wcstombs(&chars[0], widechars, len);

	chars[len] = '\0';
	return &chars[0];
}

inline const wchar_t *convertToUTF(const char *value, \
									std::vector<wchar_t> &buffer) {

	assert(value);
	size_t len = strlen(value);
	buffer.resize(len + 1);

	mbstowcs(&buffer[0], value, len);

	buffer[len] = L'\0';
	return &buffer[0];
}

inline void copyImmutableStrToVector(ImmutableString<wchar_t> str, std::vector<wchar_t> &dest) {

	assert(!str.isNull());
	dest.resize(str.size + 1);

	wcsncpy(&dest[0], str.str, str.size);
	dest[str.size] = L'\0';
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
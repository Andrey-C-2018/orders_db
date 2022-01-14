#pragma once
#include <assert.h>
#include <string>
#include <string.h>
#include <cwctype>
#include <cctype>
#include "dll_definitions.h"

#ifdef UNICODE
	typedef wchar_t Tchar;
	typedef std::wstring Tstring;
	#define _T(str) L ## str
#else
	typedef char Tchar;
	typedef std::string Tstring;
	#define _T(str) str
#endif

inline size_t Tstrlen(const char *str) noexcept {
	
	return strlen(str);
}

inline size_t Tstrlen(const wchar_t *str) noexcept {
	
	return wcslen(str);
}

inline size_t Tstrlen(const Tstring &str){
	
	return str.size();
}

inline size_t Tstrnlen(const char *str, size_t max_len) noexcept {

	return strnlen(str, max_len);
}

inline size_t Tstrnlen(const wchar_t *str, size_t max_len) noexcept {

	return wcsnlen(str, max_len);
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

inline char *Tstrcpy(char *str1, const char *str2) noexcept {
	
	return strcpy(str1, str2);
}

inline wchar_t *Tstrcpy(wchar_t *str1, const wchar_t *str2) noexcept {
	
	return wcscpy(str1, str2);
}

inline char *Tstrncpy(char *str1, const char *str2, const size_t count) noexcept {
	
	return strncpy(str1, str2, count);
}

inline wchar_t *Tstrncpy(wchar_t *str1, const wchar_t *str2, const size_t count) noexcept {
	
	return wcsncpy(str1, str2, count);
}

inline char *Tstrcat(char *str1, const char *str2) noexcept {
	
	return strcat(str1, str2);
}

inline wchar_t *Tstrcat(wchar_t *str1, const wchar_t *str2) noexcept {
	
	return wcscat(str1, str2);
}

inline int Tstrcmp(const char *str1, const char *str2) noexcept {

	return strcmp(str1, str2);
}

inline int Tstrcmp(const wchar_t *str1, const wchar_t *str2) noexcept {

	return wcscmp(str1, str2);
}

inline int Tstrncmp(const char *str1, const char *str2, const size_t max_count) noexcept {

	return strncmp(str1, str2, max_count);
}

inline int Tstrncmp(const wchar_t *str1, const wchar_t *str2, const size_t max_count) noexcept {

	return wcsncmp(str1, str2, max_count);
}

inline const char *Tstrchr(const char *str1, const int ch) noexcept {

	return strchr(str1, ch);
}

inline const wchar_t *Tstrchr(const wchar_t *str1, const wchar_t ch) noexcept {

	return wcschr(str1, ch);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

inline int ToUpper(const int ch) {

	return std::toupper(ch);
}

inline wchar_t ToUpper(const wchar_t ch) {

	return std::towupper((std::wint_t)ch);
}

inline int ToLower(const int ch) {

	return tolower(ch);
}

inline wchar_t ToLower(const wchar_t ch) {

	return towlower(ch);
}

inline const char *rtrim(char *str, const size_t len) {

	assert(str);
	assert(str[len] == '\0');
	char *p = str + len;

	while(p >= str + 1 && *(p - 1) == ' ') --p;
	*p = '\0';
	return str;
}

inline const char* ltrim(char* str, const size_t len) {

	assert(str);
	char* p = strrchr(str, ' ');
	if(p) {
		size_t gap = (size_t)(p - str + 1);
		size_t i = 0;
		while (i < len - gap && str[i + gap] != '\0') {
			str[i] = str[i + gap];
			++i;
		}
		str[len - gap] = '\0';
	}
	return str;
}

inline bool endsWith(const std::string &str, const char *suffix, const size_t suffix_len) {

	assert(suffix);
	assert(Tstrlen(suffix) >= suffix_len);

	if(!suffix_len || suffix_len > str.size()) return !suffix_len;
	return str.compare(str.size() - suffix_len, suffix_len, suffix) == 0;
}

template <typename Tchar> \
inline bool endsWith(const Tchar *str, const Tchar *suffix, const size_t suffix_len) {

	assert(str);
	assert(suffix);
	assert(Tstrlen(suffix) >= suffix_len);

	size_t str_len = Tstrlen(str);

	if(!suffix_len || suffix_len > str_len) return !suffix_len;
	return !Tstrncmp(str + str_len - suffix_len, suffix, suffix_len);
}

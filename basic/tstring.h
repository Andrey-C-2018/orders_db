#pragma once
#include <assert.h>
#include <string>
#include <string.h>
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

	return toupper(ch);
}

inline wchar_t ToUpper(const wchar_t ch) {

	return towupper(ch);
}

inline int ToLower(const int ch) {

	return tolower(ch);
}

inline wchar_t ToLower(const wchar_t ch) {

	return towlower(ch);
}
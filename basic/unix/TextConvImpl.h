#pragma once
#include <cstring>
#include <cwchar>
#include <vector>
#include <cassert>

enum{
	DEF_RESERVED_LEN = 32
};

template <class Out, typename Tchar> \
inline void addChar(Out &out, const Tchar ch){
	
	out += ch;
}

template <class Out>
inline void addZeroChar(Out &out) { }

inline void addChar(std::vector<char> &out, const char ch){
	
	out.push_back(ch);
}

inline void addChar(std::vector<wchar_t> &out, const wchar_t ch){

	out.push_back(ch);
}

inline void addZeroChar(std::vector<char> &out){

	out.push_back(0);
}

inline void addZeroChar(std::vector<wchar_t> &out){

	out.push_back(0);
}

template <class Out> \
const wchar_t *UTF8_ToUCS16(const char *in, const int len, Out &out) {

	if(!in || !len) return nullptr;

	mbstate_t state;
	wchar_t wch = L'\0';

	memset(&state, 0, sizeof state);
	out.clear();
	out.reserve(len == -1 ? DEF_RESERVED_LEN : (size_t)len + 1);
		
	const char *p_in = in;
	size_t i = 0;
	size_t rc = mbrtowc(&wch, p_in, 2, &state);
	while(i < (size_t)len && wch != L'\0' && rc && rc != (size_t)-1 && rc != (size_t)-2){
		addChar(out, wch);
		p_in += rc;
		rc = mbrtowc(&wch, p_in, 2, &state);
		++i;
	}
	addZeroChar(out);

	return rc == (size_t)-1 || out.empty() ? nullptr : &out[0];
}

template <class In, class Out> \
const wchar_t *UTF8_ToUCS16(const In &in, Out &out) {

	return UTF8_ToUCS16(&in[0], (int)in.size(), out);
}

template <class Out> \
inline const char *UCS16_ToUTF8(const wchar_t *in, const int len, Out &out) {

	if(!in || !len) return nullptr;

	mbstate_t state;
	char ch = '\0';

	memset(&state, 0, sizeof state);
	out.clear();
	out.reserve(len == -1 ? DEF_RESERVED_LEN : (size_t)len + 1);
	
	const wchar_t *p_in = in;
	size_t rc = wcrtomb(&ch, *p_in, &state);
	size_t i = 0;
	while(i < (size_t)len && ch != '\0' && rc && rc != (size_t)-1 && rc != (size_t)-2){
		addChar(out, ch);
		++p_in;
		rc = wcrtomb(&ch, *p_in, &state);
		++i;
	}
	addZeroChar(out);

	return rc == (size_t)-1 || out.empty() ? nullptr : &out[0];
}

template <class In, class Out> \
const char *UCS16_ToUTF8(const In &in, Out &out) {

	return UCS16_ToUTF8(&in[0], (int)in.size(), out);
}

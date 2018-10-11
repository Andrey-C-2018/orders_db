#pragma once
#include <string.h>
#include <wchar.h>
#include <vector>

enum{
	DEF_MAX_STR_LEN = 32
};

template <class Out> \
inline void addChar(Out &out, const char ch){
	
	out += ch;
}

template <class Out> \
inline void addChar(Out &out, const wchar_t wch){
	
	out += wch;
}

inline void addChar(std::vector<char> &out, const char ch){
	
	out.push_back(ch);
}

inline void addChar(std::vector<wchar_t> &out, const wchar_t wch){
	
	out.push_back(wch);
}

template <class Out> \
const wchar_t *UTF8_ToUCS16(const char *in, const int len, Out &out) {
	mbstate_t state;
	wchar_t wch = L'\0';
	size_t len_in = len == -1 ? DEF_MAX_STR_LEN : (size_t)len;
	
	memset(&state, 0, sizeof state);
	out.clear();
	out.reserve(len_in + 1);
		
	const char *p_in = in;
	size_t rc = mbrtowc(&wch, p_in, len_in, &state);
	while(rc && rc != (size_t)-1 && rc != (size_t)-2){
		addChar(out, wch);
		p_in += rc;
		rc = mbrtowc(&wch, p_in, len_in, &state);
	}
	
	assert(!len_in || (len_in && !rc));
	addChar(out, L'\0');
	return out.size() ? &out[0] : nullptr;
}

template <class In, class Out> \
const wchar_t *UTF8_ToUCS16(const In &in, Out &out) {

	return UTF8_ToUCS16(&in[0], (int)in.size(), out);
}

template <class Out> \
inline const char *UCS16_ToUTF8(const wchar_t *in, const int len, Out &out) {
	mbstate_t state;
	char ch = '\0';
	size_t len_in = len == -1 ? DEF_MAX_STR_LEN : (size_t)len;
	
	memset(&state, 0, sizeof state);
	out.clear();
	out.reserve(len_in + 1);
	
	const wchar_t *p_in = in;
	size_t rc = wcrtomb(&ch, *p_in, &state);
	while(rc && rc != (size_t)-1 && rc != (size_t)-2){
		addChar(out, ch);
		p_in += rc;
		rc = wcrtomb(&ch, *p_in, &state);
	}
	
	assert(!len_in || (len_in && !rc));
	addChar(out, '\0');
	return out.size() ? &out[0] : nullptr;
}

template <class In, class Out> \
const char *UCS16_ToUTF8(const In &in, Out &out) {

	return UCS16_ToUTF8(&in[0], (int)in.size(), out);
}

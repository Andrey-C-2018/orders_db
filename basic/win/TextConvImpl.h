#pragma once
#include <Windows.h>

template <class Out> \
const wchar_t *UTF8_ToUCS16(const char *in, const int len, Out &out) {
	size_t len_out(0);

	assert(in);
	len_out = MultiByteToWideChar(CP_UTF8, 0, in, len, nullptr, 0);
	assert(len_out);
	out.resize(len_out);

	MultiByteToWideChar(CP_UTF8, 0, in, len, &out[0], (int)len_out);
	return &out[0];
}

template <class In, class Out> \
const wchar_t *UTF8_ToUCS16(const In &in, Out &out) {

	return UTF8_ToUCS16(&in[0], (int)in.size(), out);
}

template <class Out> \
inline const char *UCS16_ToUTF8(const wchar_t *in, const int len, Out &out) {
	size_t len_out(0);

	len_out = WideCharToMultiByte(CP_UTF8, 0, in, len, nullptr, 0, NULL, NULL);
	assert(len_out);
	out.resize(len_out);

	WideCharToMultiByte(CP_UTF8, 0, in, len, &out[0], (int)len_out, NULL, NULL);
	return &out[0];
}

template <class In, class Out> \
const char *UCS16_ToUTF8(const In &in, Out &out) {

	return UCS16_ToUTF8(&in[0], (int)in.size(), out);
}

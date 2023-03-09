#pragma once

#ifdef _MSC_VER
#include "win/TextConvImpl.h"
#else
#include "unix/TextConvImpl.h"
#endif

inline size_t getUnicodeSymbolLength(const char ch) {

	bool d1 = (ch & 0x80) > 0;
	bool d2 = (ch & 0x40) > 0;
	bool d3 = (ch & 0x20) > 0;
	bool d4 = (ch & 0x10) > 0;
	bool d5 = (ch & 0x08) > 0;
	auto len = (size_t) (!d1);
	len += ((size_t)(d1 && d2 && !d3)) * 2;
	len += ((size_t)(d1 && d2 && d3 && !d4)) * 3;
	len += ((size_t)(d1 && d2 && d3 && d4 && !d5)) * 4;

	return len;
}

size_t cp1251_to_utf8_len(const char *text);

char *cp1251_to_utf8(const char *text, char *buffer);

#include <cassert>
#include <algorithm>
#include "TextConv.h"

const unsigned char cp1251_codepage[][3] = {{165, 210, 144}, \
											{168, 208, 129}, \
											{170, 208, 132}, \
											{175, 208, 135}, \
											{178, 208, 134}, \
											{179, 209, 150}, \
											{180, 210, 145}, \
											{184, 209, 209}, \
											{186, 209, 148}, \
											{191, 209, 151} };

unsigned findInCodepage1251(unsigned char ch) {

	size_t size = sizeof(cp1251_codepage) / (3 * sizeof(unsigned char));
	unsigned char code[3] = {ch, 0, 0};
	unsigned char const (*p)[3] = std::lower_bound(cp1251_codepage, \
												cp1251_codepage + size, code, \
					[](unsigned char const (&l)[3], unsigned char const (&r)[3]) -> bool {
		return l[0] < r[0];
	});

	bool found = p != cp1251_codepage + size && *p[0] == ch;
	unsigned wch = *(*p + 2) + *(*p + 1)*0x100;
	return found ? wch : 0;
}

size_t cp1251_to_utf8_len(const char *text) {

	assert(text);
	size_t i = 0;
	for(const char *p = text; *p != '\0'; ++p) {
		const unsigned char ch = *p;
		i += (ch >= 192 && ch <= 239) || ch > 239 || findInCodepage1251(ch);
		++i;
	}

	return i;
}

char *cp1251_to_utf8(const char *text, char *buffer) {

	assert(text);
	assert(buffer);

	size_t i = 0;
	for(const char *p = text; *p != '\0'; ++p) {
		const unsigned char ch = *p;

		if (ch >= 192 && ch <= 239) {
			buffer[i++] = (char)208;
			buffer[i++] = (char)(ch - 48);
			continue;
		}
		if (ch > 239) {
			buffer[i++] = (char)209;
			buffer[i++] = (char)(ch - 112);
			continue;
		}
		auto wch = findInCodepage1251(ch);
		if(wch) {
			buffer[i++] = (char)(wch / 0x100);
			buffer[i++] = (char)(wch % 0x100);
			continue;
		}
		buffer[i++] = (char)ch;
	}
	buffer[i] = '\0';
	return buffer;
}


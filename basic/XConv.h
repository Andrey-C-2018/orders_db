#ifndef XCONV_H
#define XCONV_H
#include <climits>
#include <cmath>
#include <string>
#include "chars.h"

class XConv {
	template <typename Tchar, typename TInt> static \
		TInt ToInt(const Tchar *str, int &error, \
					const TInt max_value);
	static inline const char *GetStrType(const char *str) { return str; }
	static inline const char *GetStrType(const std::string &str) { return str.c_str(); }
	static inline const wchar_t *GetStrType(const wchar_t *str) { return str; }
	static inline const wchar_t *GetStrType(const std::wstring &str) { return str.c_str(); }

public:
	enum Errors {
		E_CONV_NULL_STR = 1, \
		E_CONV_EMPTY_STR = 2, \
		E_CONV_WRONG_VALUE = 3, \
		E_CONV_VALUE_OUT_OF_RANGE = 4
	};
	template <typename Tchar, typename TInt> static \
		Tchar *ToString(const TInt value, Tchar *buffer, size_t &size);
		
	template <typename Tchar, typename TInt> static inline \
		Tchar *ToString(const TInt value, Tchar *buffer) {
			
		size_t size;
		return ToString(value, buffer, size);
	}
	
	template <typename String> static inline \
		int ToInt(const String str, int &error) {
			
			return ToInt(GetStrType(str), error, (int)INT_MAX);
		}
		
	template <typename String> static inline \
		unsigned ToUnsigned(const String str, int &error) {
			
			return ToInt(GetStrType(str), error, (unsigned)UINT_MAX);
		}
		
	template <typename String> static inline \
		unsigned ToSizeType(const String str, int &error) {
			
			return ToInt(GetStrType(str), error, (unsigned)SIZE_MAX);
		}
		
	template <typename String> static inline \
		long ToLong(const String str, int &error) {
			
			return ToInt(GetStrType(str), error, (long)LONG_MAX);
		}
		
	template <typename String> static inline \
		long long ToLongLong(const String str, int &error) {
			
			return ToInt(GetStrType(str), error, (long long)LLONG_MAX);
		}
	
	static inline size_t GetBufferSizeForLongLong(){
		long long l = LLONG_MAX;
		
		return (size_t)floor(std::log10((long double)l)) + 1;
	}
};

//**********************************************************************

template <typename Tchar, typename TInt> \
Tchar *XConv::ToString(const TInt value, Tchar *buffer, size_t &size) {

if (!buffer) {
	size = 0;
	return nullptr;
}
if (!value) {
	buffer[0] = getZeroChar('0');
	buffer[1] = 0;
	size = 1;
	return buffer;
}

TInt v = value;
size = 0;
if (value < 0) {
	buffer[0] = getMinusChar('-');
	v *= -1;
	++size;
}
size += (size_t)floor(std::log10((long double)v)) + 1;
buffer[size] = 0;

size_t i = size;
while (v) {
	buffer[i - 1] = getZeroChar('0') + v % 10;
	v /= 10;
	--i;
}

return buffer;
}

template <typename Tchar, typename TInt> \
TInt XConv::ToInt(const Tchar *str, int &error, \
						const TInt max_value) {

if (!str) {
	error = E_CONV_NULL_STR;
	return (TInt)-1;
}

TInt number = 0;
TInt max = max_value / 10;
size_t i = 0;
bool neg = (str[0] == _T('-'));
i += neg;

while (number < max && str[i] != 0 && \
		str[i] >= getZeroChar('0') && str[i] <= getNineChar('9')) {

	number *= 10;
	number += str[i] - getZeroChar('0');
	++i;
}

bool empty = (i == 0 && str[i] == 0);
if (empty || (str[i] != 0 && \
	!(str[i] >= getZeroChar('0') && str[i] <= getNineChar('9')))) {
	error = empty ? E_CONV_EMPTY_STR : E_CONV_WRONG_VALUE;
	number *= neg ? -1 : 1;
	return number;
}

const Tchar lowest_digit = max_value % 10;
if((number > max && str[i] != 0) || \
	(number == max && (str[i] - getZeroChar('0')) > lowest_digit)) {
	error = E_CONV_VALUE_OUT_OF_RANGE;
	number *= neg ? -1 : 1;
	return number;
}
bool key = (number == max && str[i] != 0);
number *= 10 - 9 * (!key);
number += (str[i] - getZeroChar('0')) * key;

number *= neg ? -1 : 1;
error = 0;
return number;
}

#endif // XCONV_H

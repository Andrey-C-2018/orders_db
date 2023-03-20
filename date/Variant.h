#pragma once
#include <cstddef>
#include <cassert>
#include "Date.h"

class DATELIB_DLL_EXPORT Variant {
public:
	enum Types {
		TYPE_NULL = 0,\
		TYPE_DATE = 1, \
		TYPE_STRING = 2, \
		TYPE_WSTRING = 3, \
		TYPE_INT = 4, \
		TYPE_UINT = 5
	};

private:
	Types value_type;
	union {
		int int_value;
		unsigned uint_value;
		char *str_value;
		wchar_t *wstr_value;
	};
	size_t str_size;

public:
	Variant() noexcept;

	explicit Variant(int value) noexcept;
	explicit Variant(unsigned value) noexcept;
	explicit Variant(const char *str) noexcept;
	Variant(const char *str, size_t len) noexcept;
	explicit Variant(const wchar_t *wstr) noexcept;
	Variant(const wchar_t *wstr, size_t len) noexcept;
	explicit Variant(const CDate &date) noexcept;
	Variant(unsigned char day, unsigned char month, unsigned short year) noexcept;

	Variant(const Variant &obj) noexcept;
	Variant(Variant &&obj) noexcept;
	Variant& operator=(const Variant &obj) noexcept;
	Variant& operator=(Variant &&obj) noexcept;

	inline bool isNull() const;

	inline int getInt() const;
	inline unsigned getUInt() const;
	inline const char *getString() const;
	inline const wchar_t *getWString() const;
	inline CDate getDate() const;

	inline size_t getStringLength() const;

	virtual ~Variant();
};

//**********************************************************************

bool Variant::isNull() const {

	return value_type == TYPE_NULL;
}

int Variant::getInt() const {

	assert (value_type == TYPE_INT);
	return int_value;
}

unsigned Variant::getUInt() const {

	assert (value_type == TYPE_UINT);
	return uint_value;
}

const char *Variant::getString() const {

	assert (value_type == TYPE_STRING);
	return str_value;
}

const wchar_t *Variant::getWString() const {

	assert (value_type == TYPE_WSTRING);
	return wstr_value;
}

CDate Variant::getDate() const {

	assert (value_type == TYPE_DATE);

	unsigned char day = uint_value % 0x100;
	unsigned v = uint_value >> 8;
	unsigned char month = v % 0x100;
	v >>= 8;

	return CDate(day, month, v);
}

size_t Variant::getStringLength() const {

	assert (value_type == TYPE_STRING || value_type == TYPE_WSTRING);
	return str_size;
}
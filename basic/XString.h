#pragma once
#include <assert.h>
#include "tstring.h"

template <typename Tchar> \
class XString {
	Tchar *data;
	size_t str_size, allocated_size;

	enum {
		MIN_ALLOCATION_SIZE = 10
	};

	inline void init(const Tchar *str, const size_t size);
	inline void init(const Tchar *str);
	inline void initChar(const Tchar ch);

	inline void resize(const size_t size_to_allocate, \
						const bool preserve_old_content);
	inline void destroy();
public:
	inline XString() noexcept;
	inline XString(const XString &obj);
	inline XString(XString &&obj) noexcept;
	inline XString(const Tchar *str);
	inline XString(const Tchar ch);

	inline XString &operator=(const XString &obj);
	inline XString &operator=(XString &&obj) noexcept;

	inline XString &operator=(const Tchar *str);
	inline XString &operator=(const Tchar ch);
	inline XString &assign(const Tchar *str, const size_t chars_count);
	inline XString &operator+=(const XString &str);
	inline XString &operator+=(const Tchar *str);
	inline XString &operator+=(const Tchar ch);
	inline XString &append(const Tchar *str, const size_t chars_count);

	inline Tchar &operator[](const size_t index) noexcept;
	inline const Tchar &operator[](const size_t index) const noexcept;

	inline bool operator==(const XString &obj) const noexcept;
	inline bool operator!=(const XString &obj) const noexcept;
	inline bool operator<(const XString &obj) const noexcept;
	inline bool operator<=(const XString &obj) const noexcept;
	inline bool operator>(const XString &obj) const noexcept;
	inline bool operator>=(const XString &obj) const noexcept;

	inline size_t capacity() const noexcept;
	inline size_t size() const noexcept;
	inline bool empty() const noexcept;

	inline void reserve(const size_t size);
	inline void resize(const size_t size);
	inline void clear() noexcept;

	inline const Tchar *c_str() const noexcept;
	inline Tchar *c_str_writable() noexcept;

	virtual ~XString();
};

//**********************************************************************

template <typename Tchar> \
XString<Tchar>::XString() noexcept : data(nullptr), str_size(0), \
									allocated_size(0) { }

template <typename Tchar> \
void XString<Tchar>::resize(const size_t size_to_allocate, \
										const bool preserve_old_content) {

	if (size_to_allocate > this->allocated_size) {
		size_t actual_size_to_allocate = size_to_allocate < MIN_ALLOCATION_SIZE ? \
											MIN_ALLOCATION_SIZE : size_to_allocate;
		Tchar *new_data = new Tchar[actual_size_to_allocate];

		if (str_size != 0) {
			assert(data);
			if (preserve_old_content) {
				Tstrncpy(new_data, data, str_size);
				new_data[str_size] = 0;
			}
			delete[] data;
		}
		data = new_data;
		allocated_size = actual_size_to_allocate;
	}
}

template <typename Tchar> \
void XString<Tchar>::init(const Tchar *str, const size_t size) {

	assert(str);
	assert(Tstrlen(str) >= size);
	resize(size + 1, false);

	str_size = size;
	Tstrncpy(data, str, str_size);
	data[str_size] = 0;
}

template <typename Tchar> \
void XString<Tchar>::init(const Tchar *str) {

	assert(str);
	init(str, Tstrlen(str));
}

template <typename Tchar> \
XString<Tchar>::XString(const XString &obj) : data(nullptr), str_size(0), \
														allocated_size(0) {

	init(obj.data, obj.str_size);
}

template <typename Tchar> \
XString<Tchar>::XString(XString &&obj) noexcept : data(obj.data), \
												str_size(obj.str_size), \
												allocated_size(obj.allocated_size) {

	obj.data = nullptr;
	obj.str_size = 0;
	obj.allocated_size = 0;
}

template <typename Tchar> \
XString<Tchar>::XString(const Tchar *str) : data(nullptr), str_size(0), \
													allocated_size(0) {

	init(str);
}

template <typename Tchar> \
void XString<Tchar>::initChar(const Tchar ch) {

	resize(MIN_ALLOCATION_SIZE, false);
	str_size = 1;
	data[0] = ch;
	data[1] = 0;
}

template <typename Tchar> \
XString<Tchar>::XString(const Tchar ch) : data(nullptr), str_size(0), \
													allocated_size(0) {

	initChar(ch);
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator=(const XString &obj) {

	init(obj.data, obj.str_size);
	return *this;
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator=(XString &&obj) noexcept {

	data = obj.data;
	str_size = obj.str_size;
	allocated_size = obj.allocated_size;

	obj.data = nullptr;
	obj.str_size = 0;
	obj.allocated_size = 0;
	return *this;
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator=(const Tchar *str) {

	init(str);
	return *this;
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator=(const Tchar ch) {

	initChar(ch);
	return *this;
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::assign(const Tchar *str, const size_t chars_count) {

	clear();
	return append(str, chars_count);
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator+=(const XString &str) {

	return append(str.c_str(), str.size());
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator+=(const Tchar *str) {

	return append(str, Tstrlen(str));
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::operator+=(const Tchar ch) {

	resize(str_size + 2, true);
	data[str_size] = ch;
	data[str_size + 1] = ch;

	++str_size;
	return *this;
}

template <typename Tchar> \
XString<Tchar> &XString<Tchar>::append(const Tchar *str, const size_t chars_count) {

	assert(str);
	assert(Tstrlen(str) >= chars_count);

	resize(str_size + chars_count + 1, true);
	Tstrncpy(data + str_size, str, chars_count);

	str_size += chars_count;
	data[str_size] = 0;
	return *this;
}

template <typename Tchar> \
Tchar &XString<Tchar>::operator[](const size_t index) noexcept {

	assert(index < str_size);
	return data[index];
}

template <typename Tchar> \
const Tchar &XString<Tchar>::operator[](const size_t index) const noexcept {

	assert(index < str_size);
	return data[index];
}

template <typename Tchar> \
bool XString<Tchar>::operator==(const XString &obj) const noexcept {
	size_t l_size = str_size;
	size_t r_size = obj.str_size;

	return l_size == r_size && \
		((l_size && r_size && !Tstrcmp(data, obj.data)) || \
			!l_size);
}

template <typename Tchar> \
bool XString<Tchar>::operator!=(const XString &obj) const noexcept {

	return !operator==(obj);
}

template <typename Tchar> \
bool XString<Tchar>::operator<(const XString &obj) const noexcept {
	size_t l_size = str_size;
	size_t r_size = obj.str_size;

	return (!l_size && r_size) || \
		(l_size && r_size && (Tstrcmp(data, obj.data) < 0));
}

template <typename Tchar> \
bool XString<Tchar>::operator<=(const XString &obj) const noexcept {

	return !operator>(obj);
}

template <typename Tchar> \
bool XString<Tchar>::operator>(const XString &obj) const noexcept {
	size_t l_size = str_size;
	size_t r_size = obj.str_size;

	return (l_size && !r_size) || \
		(l_size && r_size && (Tstrcmp(data, obj.data) > 0));
}

template <typename Tchar> \
bool XString<Tchar>::operator>=(const XString &obj) const noexcept {

	return !operator<(obj);
}

template <typename Tchar> \
size_t XString<Tchar>::capacity() const noexcept {

	return this->allocated_size;
}

template <typename Tchar> \
size_t XString<Tchar>::size() const noexcept {

	return str_size;
}

template <typename Tchar> \
bool XString<Tchar>::empty() const noexcept {

	return str_size == 0;
}

template <typename Tchar> \
void XString<Tchar>::reserve(const size_t size) {

	resize(size + 1, true);
}

template <typename Tchar> \
void XString<Tchar>::resize(const size_t size) {

	reserve(size);
	this->str_size = size;
	data[size] = 0;
}

template <typename Tchar> \
void XString<Tchar>::clear() noexcept {

	if (str_size != 0) data[0] = 0;
	str_size = 0;
}

template <typename Tchar> \
const Tchar *XString<Tchar>::c_str() const noexcept {

	return data;
}

template <typename Tchar> \
Tchar *XString<Tchar>::c_str_writable() noexcept {

	return data;
}

template <typename Tchar> \
void XString<Tchar>::destroy() {

	if (str_size) {
		assert(data);

		delete[] data;
		data = nullptr;
		str_size = 0;
		allocated_size = 0;
	}
}

template <typename Tchar> \
XString<Tchar>::~XString() {

	destroy();
}

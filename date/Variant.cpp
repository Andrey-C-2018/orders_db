#include <cstring>
#include <cwchar>
#include <memory>
#include "Variant.h"

Variant::Variant() noexcept : value_type(TYPE_NULL), int_value(0), str_size(0) { }

Variant::Variant(int value) noexcept : value_type(TYPE_INT), \
										int_value(value), str_size(0) { }

Variant::Variant(unsigned value) noexcept : value_type(TYPE_UINT), \
											unsigned_value(value), str_size(0) { }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

Variant::Variant(const char *str) noexcept : value_type(TYPE_STRING), \
											str_value(nullptr), str_size(0) {

	if (str != nullptr) {
		str_size = strlen(str);
		str_value = new char[str_size + 1];
		strcpy(str_value, str);
	}
}

Variant::Variant(const char *str, size_t len) noexcept : value_type(TYPE_STRING), \
											str_value(nullptr), str_size(len) {

	assert (!str || len > 0);
	if (str != nullptr) {

		assert(len == strlen(str));
		str_value = new char[str_size + 1];
		strcpy(str_value, str);
	}
}

Variant::Variant(const wchar_t *wstr) noexcept : value_type(TYPE_WSTRING), \
												wstr_value(nullptr), str_size(0) {

	if (wstr != nullptr) {
		str_size = wcslen(wstr);
		wstr_value = new wchar_t[str_size + 1];
		wcscpy(wstr_value, wstr);
	}
}

Variant::Variant(const wchar_t *wstr, size_t len) noexcept : value_type(TYPE_WSTRING), \
												wstr_value(nullptr), str_size(len) {

	assert (!wstr || len > 0);
	if (wstr != nullptr) {

		assert(len == wcslen(wstr));
		wstr_value = new wchar_t[str_size + 1];
		wcscpy(wstr_value, wstr);
	}
}

Variant::Variant(CDate date) noexcept : value_type(TYPE_DATE), \
												date_value(std::move(date)), \
												str_size(0) { }

Variant::Variant(const Variant &obj) noexcept : value_type(obj.value_type), \
												str_size(obj.str_size) {

	if (obj.value_type == TYPE_STRING) {
		str_value = nullptr;
		if (obj.str_value != nullptr) {
			str_value = new char[str_size + 1];
			strcpy(str_value, obj.str_value);
		}
	}

	if (obj.value_type == TYPE_WSTRING) {
		wstr_value = nullptr;
		if (obj.wstr_value != nullptr) {
			wstr_value = new wchar_t[str_size + 1];
			wcscpy(wstr_value, obj.wstr_value);
		}
	}
}

Variant::Variant(Variant &&obj) noexcept : value_type(obj.value_type), \
											str_value(obj.str_value), \
											str_size(obj.str_size) {

	obj.value_type = TYPE_NULL;
	obj.str_value = nullptr;
	obj.str_size = 0;
	assert (obj.wstr_value == nullptr);
}

Variant &Variant::operator=(const Variant &obj) noexcept {

	value_type = obj.value_type;
	str_size = obj.str_size;

	if (obj.value_type == TYPE_STRING) {
		str_value = nullptr;
		if (obj.str_value != nullptr) {
			str_value = new char[str_size + 1];
			strcpy(str_value, obj.str_value);
		}
	}

	if (obj.value_type == TYPE_WSTRING) {
		wstr_value = nullptr;
		if (obj.wstr_value != nullptr) {
			wstr_value = new wchar_t[str_size + 1];
			wcscpy(wstr_value, obj.wstr_value);
		}
	}
	return *this;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

Variant &Variant::operator=(Variant &&obj) noexcept {

	value_type = obj.value_type;
	str_value = obj.str_value;
	str_size = obj.str_size;

	obj.value_type = TYPE_NULL;
	obj.str_value = nullptr;
	obj.str_size = 0;
	assert (obj.wstr_value == nullptr);
	return *this;
}

Variant::~Variant() {

	if (value_type == TYPE_STRING)
		if (str_value != nullptr)
			delete [] str_value;

	if (value_type == TYPE_WSTRING)
		if (wstr_value != nullptr)
			delete [] wstr_value;
}

#pragma once
#include <string>
#include "dll_definitions.h"

template <typename Tchar_> \
struct BASIC_DLL_EXPORT ImmutableString final {
	const Tchar_ *str;
	size_t size;

	ImmutableString(const ImmutableString &obj) noexcept : \
						str(obj.str), size(obj.size) { }

	ImmutableString(const Tchar_ *str) = delete;

	ImmutableString(const Tchar_ *str_, const size_t size_) noexcept : \
						str(str_), size(size_){ }

	template <class Tstring_> \
		explicit ImmutableString(const Tstring_ &str_) noexcept : \
						str(str_.c_str()), size(str_.size()){ }

	ImmutableString &operator=(const ImmutableString &obj) noexcept {

		this->str = obj.str;
		this->size = obj.size;
		return *this;
	}

	inline ImmutableString &operator=(const Tchar_ *str) = delete;

	template <class Tstring_> \
		inline ImmutableString &operator=(const Tstring_ &str_) noexcept {

		this->str = str.c_str();
		this->size = str_.size();
		return *this;
	}

	inline bool isNull() const noexcept { return str == nullptr || !size; }
};
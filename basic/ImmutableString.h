#pragma once

template <typename Tchar_> \
struct ImmutableString final {
	const Tchar_ *str;
	const size_t size;

	ImmutableString(const Tchar_ *str_, const size_t size_) noexcept : \
						str(str_), size(size_){ }
	template <class Tstring_> \
		ImmutableString(const Tstring_ &str_) noexcept : \
						str(str_.c_str()), size(str_.size()){ }

	inline bool isNull() const { return str == nullptr || !size; }
};
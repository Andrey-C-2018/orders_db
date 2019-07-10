#pragma once
#include <ostream>
#include <basic/XConv.h>

class CCurrency{
	enum{
		MAX_DIGITS_COUNT = 7, \
		MAX_CENTS_DIGITS_COUNT = 2
	};
	
	size_t curr;
	bool is_null;
	
	static const size_t cents_multiplier;
public:
	enum Errors {
		E_NULL_STR = XConv::E_CONV_NULL_STR, \
		E_EMPTY_STR = XConv::E_CONV_EMPTY_STR, \
		E_WRONG_VALUE = XConv::E_CONV_WRONG_VALUE, \
		E_VALUE_OUT_OF_RANGE = XConv::E_CONV_VALUE_OUT_OF_RANGE, \
		E_CENTS_OUT_OF_RANGE = E_VALUE_OUT_OF_RANGE + 1
	};

	CCurrency() noexcept;
	CCurrency(const char *value) noexcept;
	CCurrency(const CCurrency &obj) = default;
	CCurrency(CCurrency &&obj) = default;
	CCurrency &operator=(const CCurrency &obj) = default;
	CCurrency &operator=(CCurrency &&obj) = default;

	int init(const char *value) noexcept;
	inline bool operator==(const CCurrency &obj) const noexcept;
	size_t getDifference(const CCurrency &obj) const noexcept;
	friend std::ostream &operator<< (std::ostream &out, const CCurrency &obj);

	virtual ~CCurrency() noexcept;
};

bool CCurrency::operator==(const CCurrency &obj) const noexcept {

	return curr == obj.curr;
}


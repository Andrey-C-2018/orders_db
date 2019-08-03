#include "Currency.h"

const size_t CCurrency::cents_multiplier = (size_t)std::pow((double)10, \
															(double)MAX_CENTS_DIGITS_COUNT);

CCurrency::CCurrency() noexcept : curr(0), is_null(true){ }

CCurrency::CCurrency(const char *value) noexcept : curr(0), is_null(true){

	init(value);
}

int CCurrency::init(const char *value) noexcept {
	auto is_number = [](const char ch) {
		return ch >= '0' && ch <= '9';
	};
	auto getDigitsCount = [](const size_t number) {
		size_t result = (size_t)floor(std::log10((long double)number));
		return (number != 0) * result + 1;
	};

	int error = 0;
	const char *p_value = value;

	p_value += (value && *value == '-');

	size_t num = XConv::ToUnsigned(p_value, error);
	size_t digits = p_value && is_number(p_value[0]) ? \
					getDigitsCount(num) : 0;

	bool no_dec_part = !digits || \
						(digits > 0 && p_value[digits] != '.' && p_value[digits] != ',');
	if (error == XConv::E_CONV_VALUE_OUT_OF_RANGE || digits > MAX_DIGITS_COUNT || \
		(error == XConv::E_CONV_WRONG_VALUE && no_dec_part) || \
		error == XConv::E_CONV_EMPTY_STR || error == XConv::E_CONV_NULL_STR) {

		error = digits > MAX_DIGITS_COUNT ? E_VALUE_OUT_OF_RANGE : error;
		this->curr = 0;
		this->is_null = true;
		return error;
	}

	size_t exact_multiplier = cents_multiplier;
	if (error == XConv::E_CONV_WRONG_VALUE && !no_dec_part) {

		size_t i = 0;
		while (i < MAX_CENTS_DIGITS_COUNT && is_number(p_value[digits + i + 1])) {
			num *= 10;
			num += p_value[digits + i + 1] - '0';
			++i;
			exact_multiplier /= 10;
		}

		if ((i && p_value[digits + i + 1] != '\0') || \
			(!i && !is_number(p_value[digits + i + 1]))) {

			error = is_number(p_value[digits + i + 1]) ? \
						E_CENTS_OUT_OF_RANGE : error;
			this->curr = 0;
			this->is_null = true;
			return error;
		}
	}
	num *= exact_multiplier;

	this->curr = num;
	this->is_null = false;
	return 0;
}

size_t CCurrency::getDifference(const CCurrency &obj) const noexcept{
	
	return curr > obj.curr ? curr - obj.curr : obj.curr - curr;
}

std::ostream &operator<< (std::ostream &out, const CCurrency &obj) {

	if (obj.is_null)
		out << "NULL";
	else{
		out<< (size_t)(obj.curr / 100)<< '.';
	
		size_t cents = obj.curr % 100;
		if(cents < 10) out<< '0';
		out<< cents;
	}

	return out;
}

CCurrency::~CCurrency() noexcept{ }
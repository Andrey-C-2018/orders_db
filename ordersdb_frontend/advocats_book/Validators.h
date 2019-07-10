#pragma once
#include <basic/tstring.h>
#include <basic/ImmutableString.h>

class CPostIndexValidator {
public:
	static inline void validate(ImmutableString<Tchar> post_index_str, \
								Tstring &error_str) {

		size_t &size = post_index_str.size;

		if (size < 3 || size > 5) 
			error_str += _T("Кількість цифр у поштовому індексі - від 3 до 5\n");
	}
};
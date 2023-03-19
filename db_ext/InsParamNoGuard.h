#pragma once
#include <cassert>

class CInsParamNoGuard final {
	size_t &param_no;
	const size_t inc;

public:
	inline CInsParamNoGuard(size_t &param_no_, const size_t inc_) noexcept : \
		param_no(param_no_), inc(inc_) {

		assert(inc > 0);
	}

	CInsParamNoGuard(const CInsParamNoGuard &obj) = delete;
	CInsParamNoGuard(CInsParamNoGuard &&obj) = default;
	CInsParamNoGuard &operator=(const CInsParamNoGuard &obj) = delete;
	CInsParamNoGuard &operator=(CInsParamNoGuard &&obj) = default;

	~CInsParamNoGuard() {

		param_no += inc;
	}
};

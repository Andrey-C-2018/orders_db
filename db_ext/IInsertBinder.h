#pragma once
#include <memory>
#include <basic/tstring.h>

struct IDbBindingTarget;

struct IInsertBinder {
	struct Params {
		size_t param_no;
		Tstring error_str;
	};

	virtual bool bind(std::shared_ptr<IDbBindingTarget> binding_target, \
						Params &params, const Tchar *field_name) = 0;
	virtual ~IInsertBinder() { }
};
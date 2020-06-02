#pragma once
#include <memory>

struct IDbBindingTarget;

struct IBinder {
	virtual size_t bind(std::shared_ptr<IDbBindingTarget> binding_target, \
						const size_t param_no) = 0;

	virtual size_t affectedParamsCount() const = 0;

	virtual ~IBinder() { }
};
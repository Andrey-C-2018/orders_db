#pragma once
#include <memory>

struct IDbBindingTarget;

struct IBinder {
	virtual void bind(std::shared_ptr<IDbBindingTarget> binding_target, const size_t param_no) = 0;
	virtual ~IBinder() { }
};
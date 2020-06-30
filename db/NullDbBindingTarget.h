#pragma once
#include "IDbBindingTarget.h"

class CNullDbBindingTarget : public IDbBindingTarget {
public:
	CNullDbBindingTarget() { }

	CNullDbBindingTarget(const CNullDbBindingTarget &obj) = default;
	CNullDbBindingTarget(CNullDbBindingTarget &&obj) = default;
	CNullDbBindingTarget &operator=(const CNullDbBindingTarget &obj) = default;
	CNullDbBindingTarget &operator=(CNullDbBindingTarget &&obj) = default;

	void bindValue(const size_t param_no, const int value) override { }

	void bindValue(const size_t param_no, const char *value) override { }
	void bindValue(const size_t param_no, const wchar_t *value) override { }
	void bindValue(const size_t param_no, \
					const ImmutableString<char> value) override { }
	void bindValue(const size_t param_no, \
					const ImmutableString<wchar_t> value) override { }

	void bindValue(const size_t param_no, const CDate &value) override { }
	void bindNull(const size_t param_no) override { }

	virtual ~CNullDbBindingTarget() { }
};


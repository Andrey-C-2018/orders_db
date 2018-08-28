#pragma once
#include <basic/ImmutableString.h>
#include <date/Date.h>

struct IDbBindingTarget {
	virtual void bindValue(const size_t param_no, const int value) = 0;

	virtual void bindValue(const size_t param_no, const char *value) = 0;
	virtual void bindValue(const size_t param_no, const wchar_t *value) = 0;
	virtual void bindValue(const size_t param_no, \
							const ImmutableString<char> value) = 0;
	virtual void bindValue(const size_t param_no, \
							const ImmutableString<wchar_t> value) = 0;

	virtual void bindValue(const size_t param_no, const CDate &value) = 0;
	virtual void bindNull(const size_t param_no) = 0;
	virtual ~IDbBindingTarget() { }
};

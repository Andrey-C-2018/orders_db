#pragma once
#include <memory>
#include <basic/ImmutableString.h>
#include <date/Date.h>
#include "IDbCommon.h"

struct IDbResultSet;

struct IDbStatement {
	virtual void bindValue(const size_t param_no, const int value) = 0;

	virtual void bindValue(const size_t param_no, const char *value) = 0;
	virtual void bindValue(const size_t param_no, const wchar_t *value) = 0;
	virtual void bindValue(const size_t param_no, \
							const ImmutableString<char> value) = 0;
	virtual void bindValue(const size_t param_no, \
							const ImmutableString<wchar_t> value) = 0;

	virtual void bindValue(const size_t param_no, const CDate &value) = 0;

	virtual std::shared_ptr<IDbResultSet> exec() = 0;
	virtual record_t execScalar() = 0;
	virtual ~IDbStatement() { }
};

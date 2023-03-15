#pragma once
#include <memory>
#include <basic/ImmutableString.h>
#include <date/Date.h>

struct IDbBindingTarget;

struct IDbResultSet {
	virtual bool empty() const = 0;
	virtual size_t getFieldsCount() const = 0;
	virtual size_t getRecordsCount() const = 0;

	virtual void gotoRecord(const size_t record) const = 0;

	virtual int getInt(const size_t field, bool &is_null) const = 0;
	virtual const char *getString(const size_t field) const = 0;
	virtual const wchar_t *getWString(const size_t field) const = 0;
	virtual ImmutableString<char> getImmutableString(const size_t field) const = 0;
	virtual ImmutableString<wchar_t> getImmutableWString(const size_t field) const = 0;
	virtual CDate getDate(const size_t field, bool &is_null) const = 0;

	virtual void reload() = 0;
	virtual std::shared_ptr<IDbResultSet> staticClone() const = 0;

	virtual ~IDbResultSet() { }
};
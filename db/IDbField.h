#pragma once
#include <memory>
#include <basic/ImmutableString.h>

struct IDbResultSet;
struct IDbBindingTarget;

struct IDbField {
	virtual ImmutableString<char> getFieldName() const = 0;
	virtual ImmutableString<wchar_t> getFieldNameW() const = 0;
	virtual size_t getFieldMaxLength() const = 0;
	virtual ImmutableString<char> getTableName() const = 0;
	virtual ImmutableString<wchar_t> getTableNameW() const = 0;
	virtual bool isPrimaryKey() const = 0;

	virtual const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;
	virtual const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;
	virtual ImmutableString<char> getValueAsImmutableString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;
	virtual ImmutableString<wchar_t> getValueAsImmutableWString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;

	virtual void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no, \
									const char *value) const = 0;
	virtual void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no, \
									const wchar_t *value) const = 0;

	virtual void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
									std::shared_ptr<IDbBindingTarget> binding_target, \
									const size_t param_no) const = 0;

	virtual ~IDbField() { }
};
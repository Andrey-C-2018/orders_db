#pragma once
#include <memory>

template <typename Tchar_> \
struct CDbFieldProperties {
	const Tchar_ *field_name;
	const Tchar_ *table_name;
	bool is_primary_key;
};

struct IDbResultSet;
struct IDbBindingTarget;

struct IDbField {
	virtual CDbFieldProperties<char> getFieldProperties() const = 0;
	virtual CDbFieldProperties<wchar_t> getFieldPropertiesW() const = 0;

	virtual const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;
	virtual const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const = 0;

	virtual void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
									const char *value) const = 0;
	virtual void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
									const wchar_t *value) const = 0;

	virtual ~IDbField() { }
};
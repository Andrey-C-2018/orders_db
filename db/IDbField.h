#pragma once
#include <memory>
#include <basic/ImmutableString.h>

template <typename Tstring_> \
struct CDbFieldProperties {
	Tstring_ field_name;
	Tstring_ table_name;
	size_t field_size;
	bool is_primary_key;

	typedef typename Tstring_::value_type Tchar_;

	CDbFieldProperties() : field_size(0), is_primary_key(false) { }
	CDbFieldProperties(const CDbFieldProperties &obj) = default;
	CDbFieldProperties(CDbFieldProperties &&obj) = default;
	CDbFieldProperties &operator=(const CDbFieldProperties &obj) = default;
	CDbFieldProperties &operator=(CDbFieldProperties &&obj) = default;
	~CDbFieldProperties() { }
};

struct IDbResultSet;
struct IDbBindingTarget;

struct IDbField {
	virtual CDbFieldProperties<std::string> getFieldProperties() const = 0;
	virtual CDbFieldProperties<std::wstring> getFieldPropertiesW() const = 0;

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
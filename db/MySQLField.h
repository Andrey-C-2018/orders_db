#pragma once
#include <basic/XConv.h>
#include "IDbField.h"
#include "IDbBindingTarget.h"
#include "MySQLResultSet.h"

class CMySQLField : public IDbField{
	std::shared_ptr<MYSQL_RES> metadata;
	
	mutable XString<wchar_t> conv_buffer;
	inline const char *ConvertIfNecessary(const char *str, char type_hint) const;
	inline const wchar_t *ConvertIfNecessary(const char *str, wchar_t type_hint) const;

protected:
	const size_t field;
	template <class Tstring_> \
		inline void TgetField(CDbFieldProperties<Tstring_> &field_props) const;

	//inline int getCellValueOfType(const size_t field, int type_hint) const;
public:
	CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	CDbFieldProperties<std::string> getFieldProperties() const override;
	CDbFieldProperties<std::wstring> getFieldPropertiesW() const override;
	
	virtual ~CMySQLField();
};

template <typename Tint> \
class CMySQLIntegerField : public CMySQLField {
	mutable wchar_t buffer[getDigitsCountOfType<Tint>() + 1];
public:
	CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	CDbFieldProperties<std::string> getFieldProperties() const override;
	CDbFieldProperties<std::wstring> getFieldPropertiesW() const override;

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLIntegerField();
};

class CMySQLDateField : public CMySQLField {
	mutable wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CMySQLDateField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	CDbFieldProperties<std::string> getFieldProperties() const override;
	CDbFieldProperties<std::wstring> getFieldPropertiesW() const override;

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLDateField();
};

class CMySQLStringField : public CMySQLField {
	wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CMySQLStringField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLStringField();
};

//**************************************************

const char *CMySQLField::ConvertIfNecessary(const char *str, char type_hint) const {

	return str;
}

const wchar_t *CMySQLField::ConvertIfNecessary(const char *str, wchar_t type_hint) const {

	return UTF8_ToUCS16(str, -1, conv_buffer);
}

template <class Tstring_> void CMySQLField::TgetField(CDbFieldProperties<Tstring_> &field_props) const {
	
	mysql_field_seek(metadata.get(), field);

	MYSQL_FIELD *mysql_field = mysql_fetch_field(metadata.get());
	field_props.field_name = ConvertIfNecessary(mysql_field->name, Tstring_::value_type());
	field_props.table_name = ConvertIfNecessary(mysql_field->table, Tstring_::value_type());
	field_props.field_size = mysql_field->length;
	field_props.is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;
}

//**************************************************

template <typename Tint> \
CMySQLIntegerField<Tint>::CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
												CMySQLField(metadata_, field_) { }

template <typename Tint> \
CDbFieldProperties<std::string> CMySQLIntegerField<Tint>::getFieldProperties() const {
	CDbFieldProperties<std::string> field_props_item;

	TgetField<std::string>(field_props_item);
	field_props_item.field_size = getDigitsCountOfType<Tint>();

	return field_props_item;
}

template <typename Tint> \
CDbFieldProperties<std::wstring> CMySQLIntegerField<Tint>::getFieldPropertiesW() const {
	CDbFieldProperties<std::wstring> field_props_item;

	TgetField<std::wstring>(field_props_item);
	field_props_item.field_size = getDigitsCountOfType<Tint>();

	return field_props_item;
}

template <typename Tint> \
const char *CMySQLIntegerField<Tint>::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	return XConv::ToString(result_set->getInt(field), p);
}

template <typename Tint> \
const wchar_t *CMySQLIntegerField<Tint>::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return XConv::ToString(result_set->getInt(field), buffer);
}

template <typename Tint> \
ImmutableString<char> CMySQLIntegerField<Tint>::getValueAsImmutableString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;
	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), p, size);
	return ImmutableString<char>(str, size);
}

template <typename Tint> \
ImmutableString<wchar_t> CMySQLIntegerField<Tint>::getValueAsImmutableWString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {
	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), buffer, size);
	return ImmutableString<wchar_t>(str, size);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const char *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	assert(!err);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const wchar_t *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	assert(!err);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
												std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no) const {
	Tint i = result_set->getInt(field);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
CMySQLIntegerField<Tint>::~CMySQLIntegerField() { }

#pragma once
#include "IDbField.h"
#include "MySQLCommon.h"

class CMySQLField : public IDbField{
	std::shared_ptr<MYSQL_RES> metadata;
	size_t field;

	mutable XString<wchar_t> conv_buffer;
	inline const char *ConvertIfNecessary(const char *str, char type_hint) const;
	inline const wchar_t *ConvertIfNecessary(const char *str, wchar_t type_hint) const;

	template <class Tchar_> inline CDbField<Tchar_> TgetField() const;
public:
	CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	CDbFieldProperties<char> getFieldProperties() const override;
	CDbFieldProperties<wchar_t> getFieldPropertiesW() const override;

	virtual ~CMySQLField();
};

class CMySQLIntegerField : public CMySQLField {
public:
	CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const wchar_t *value) const override;

	virtual ~CMySQLIntegerField();
};

//**************************************************

const char *CMySQLField::ConvertIfNecessary(const char *str, char type_hint) const {

	return str;
}

const wchar_t *CMySQLField::ConvertIfNecessary(const char *str, wchar_t type_hint) const {

	return UTF8_ToUCS16(str, -1, conv_buffer);
}

template <class Tchar_> \
CDbField<Tchar_> CMySQLField::TgetField() const {
	CDbField<Tchar_> field_item;

	assert(field < fields_count);
	mysql_field_seek(metadata.get(), field);

	MYSQL_FIELD *mysql_field = mysql_fetch_field(metadata.get());
	field_item.field_name = ConvertIfNecessary(mysql_field->name, Tchar_());
	field_item.table_name = ConvertIfNecessary(mysql_field->table, Tchar_());
	field_item.is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;

	return field_item;
}

#pragma once
#include <memory>
#include <basic/XString.h>
#include "IDbResultSetMetadata.h"
#include "MySQLCommon.h"

class CMySQLResultSetMetadata :	public IDbResultSetMetadata{
	std::shared_ptr<MYSQL_RES> metadata;
	size_t fields_count;
	
	mutable XString<wchar_t> conv_buffer;
	inline const char *ConvertIfNecessary(const char *str, char type_hint) const;
	inline const wchar_t *ConvertIfNecessary(const char *str, wchar_t type_hint) const;

	template <class Tchar_> \
		inline CDbField<Tchar_> TgetField(const size_t field) const;
public:
	CMySQLResultSetMetadata(std::shared_ptr<MYSQL_RES> metadata_);
	size_t getFieldsCount() const override;
	CDbField<char> getField(const size_t field) const override;
	CDbField<wchar_t> getFieldW(const size_t field) const override;
	virtual ~CMySQLResultSetMetadata();
};

//**************************************************

const char *CMySQLResultSetMetadata::ConvertIfNecessary(const char *str, char type_hint) const{

	return str;
}

const wchar_t *CMySQLResultSetMetadata::ConvertIfNecessary(const char *str, wchar_t type_hint) const {

	return UTF8_ToUCS16(str, -1, conv_buffer);
}

template <class Tchar_> \
CDbField<Tchar_> CMySQLResultSetMetadata::TgetField(const size_t field) const {
	CDbField<Tchar_> field_item;

	assert(field < fields_count);
	mysql_field_seek(metadata.get(), field);

	MYSQL_FIELD *mysql_field = mysql_fetch_field(metadata.get());
	field_item.field_name = ConvertIfNecessary(mysql_field->name, Tchar_());
	field_item.table_name = ConvertIfNecessary(mysql_field->table, Tchar_());
	field_item.is_primary_key = (mysql_field->flags | PRI_KEY_FLAG) > 0;

	return field_item;
}
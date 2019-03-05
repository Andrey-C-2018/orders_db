#include "MySQLField.h"

CMySQLFieldException::CMySQLFieldException(const int err_code, \
											const Tchar *err_descr) : \
											CMySQLException(err_code, err_descr) { }

CMySQLFieldException::CMySQLFieldException(MYSQL *conn) : \
											CMySQLException(conn) { }

CMySQLFieldException::CMySQLFieldException(MYSQL_STMT *stmt) : \
											CMySQLException(stmt) { }

CMySQLFieldException::CMySQLFieldException(const CMySQLFieldException &obj) : \
											CMySQLException(obj) { }

CMySQLFieldException::~CMySQLFieldException() { }

//*************************************************************

CMySQLField::CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, \
							const size_t field_, const size_t max_field_size) : \
							metadata(metadata_), max_size(max_field_size), \
							field(field_) {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;
}

CMySQLField::CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, \
							const size_t field_) : \
							metadata(metadata_), field(field_) {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	max_size = mysql_field->length;
	is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
ImmutableString<char> CMySQLField::getFieldName() const {

	if (field_name.empty()) {
		MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
		field_name.resize(mysql_field->name_length + 1);
		strncpy(&field_name[0], mysql_field->name, mysql_field->name_length);
	}
	return ImmutableString<char>(&field_name[0], field_name.size() - 1);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

ImmutableString<wchar_t> CMySQLField::getFieldNameW() const {

	if (field_name_w.empty()) {
		MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
		UTF8_ToUCS16(mysql_field->name, mysql_field->name_length, field_name_w);
		field_name_w.push_back(0);
	}
	return ImmutableString<wchar_t>(&field_name_w[0], field_name_w.size() - 1);
}

size_t CMySQLField::getFieldMaxLength() const {

	return max_size;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
ImmutableString<char> CMySQLField::getTableName() const {

	if (table_name.empty()) {
		MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
		table_name.resize(mysql_field->table_length + 1);
		strncpy(&table_name[0], mysql_field->table, mysql_field->table_length);
	}
	return ImmutableString<char>(&table_name[0], table_name.size() - 1);
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

ImmutableString<wchar_t> CMySQLField::getTableNameW() const {

	if (table_name_w.empty()) {
		MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
		UTF8_ToUCS16(mysql_field->table, mysql_field->table_length, table_name_w);
		table_name_w.push_back(0);
	}
	return ImmutableString<wchar_t>(&table_name_w[0], table_name_w.size() - 1);
}

bool CMySQLField::isPrimaryKey() const {

	return is_primary_key;
}

CMySQLField::~CMySQLField() { }

//**************************************************

CMySQLDateField::CMySQLDateField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
								CMySQLField(metadata_, field_, CDate::GERMAN_FORMAT_LEN) { }

const char *CMySQLDateField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return p;
}

const wchar_t *CMySQLDateField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return buffer;
}

ImmutableString<char> CMySQLDateField::getValueAsImmutableString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return ImmutableString<char>(p, CDate::GERMAN_FORMAT_LEN);
}

ImmutableString<wchar_t> CMySQLDateField::getValueAsImmutableWString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return ImmutableString<wchar_t>(buffer, CDate::GERMAN_FORMAT_LEN);
}

void CMySQLDateField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const char *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const wchar_t *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
										std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no) const {

	CDate date_value = result_set->getDate(field);
	binding_target->bindValue(param_no, date_value);
}

CMySQLDateField::~CMySQLDateField() { }

//**************************************************

CMySQLStringField::CMySQLStringField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
									CMySQLField(metadata_, field_) { }

const char *CMySQLStringField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getString(field);
}

const wchar_t *CMySQLStringField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	return result_set->getWString(field);
}

ImmutableString<char> CMySQLStringField::getValueAsImmutableString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableString(field);
}

ImmutableString<wchar_t> CMySQLStringField::getValueAsImmutableWString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableWString(field);
}

void CMySQLStringField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const char *value) const {
	
	assert(value);
	binding_target->bindValue(param_no, value);
}

void CMySQLStringField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const wchar_t *value) const {
	
	assert(value);
	binding_target->bindValue(param_no, value);
}

void CMySQLStringField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
											std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no) const {

	const char *value = result_set->getString(field);
	binding_target->bindValue(param_no, value);
}

CMySQLStringField::~CMySQLStringField() { }
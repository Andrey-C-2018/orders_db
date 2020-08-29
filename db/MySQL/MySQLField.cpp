#include <vector>
#include <basic/TextConv.h>
#include "MySQLField.h"
#include "MySQLStmtData.h"

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

CMySQLField::CMySQLField(std::weak_ptr<const MySQLStmtData> stmt_, \
							const size_t field_, const size_t max_field_size) : \
							stmt(stmt_), max_size(max_field_size), \
							field(field_) {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;
}

MYSQL_FIELD *CMySQLField::getMySQLFieldHandle() const {

	std::shared_ptr<const MySQLStmtData> sh_stmt = stmt.lock();
	if (!sh_stmt)
		throw CMySQLFieldException(CMySQLFieldException::E_STMT_RELEASED, \
			_T("the statement this field belongs to has been already released"));

	assert(sh_stmt->metadata);
	mysql_field_seek(sh_stmt->metadata, (MYSQL_FIELD_OFFSET)field);
	return mysql_fetch_field(sh_stmt->metadata);
}

CMySQLField::CMySQLField(std::weak_ptr<const MySQLStmtData> stmt_, \
							const size_t field_) : \
							stmt(stmt_), field(field_) {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	max_size = mysql_field->length;
	is_primary_key = (mysql_field->flags & PRI_KEY_FLAG) > 0;
}

std::string CMySQLField::getFieldName() const {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	return std::string(mysql_field->org_name);
}

std::wstring CMySQLField::getFieldNameW() const {

	std::vector<wchar_t> field_name_w;
	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();

	UTF8_ToUCS16(mysql_field->org_name, mysql_field->org_name_length, field_name_w);
	field_name_w.push_back(0);
	
	return std::wstring(&field_name_w[0]);
}

std::string CMySQLField::getFieldAlias() const {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	return std::string(mysql_field->name);
}

std::wstring CMySQLField::getFieldAliasW() const {

	std::vector<wchar_t> field_name_w;
	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();

	UTF8_ToUCS16(mysql_field->name, mysql_field->name_length, field_name_w);
	field_name_w.push_back(0);

	return std::wstring(&field_name_w[0]);
}

size_t CMySQLField::getFieldMaxLength() const {

	return max_size;
}

std::string CMySQLField::getTableName() const {

	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();
	return std::string(mysql_field->org_table);
}

std::wstring CMySQLField::getTableNameW() const {

	std::vector<wchar_t> table_name_w;
	MYSQL_FIELD *mysql_field = this->getMySQLFieldHandle();

	UTF8_ToUCS16(mysql_field->org_table, mysql_field->org_table_length, table_name_w);
	table_name_w.push_back(0);

	return std::wstring(&table_name_w[0]);
}

bool CMySQLField::isPrimaryKey() const {

	return is_primary_key;
}

CMySQLField::~CMySQLField() { }

//**************************************************

CMySQLDateField::CMySQLDateField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_) : \
								CMySQLField(stmt_, field_, CDate::GERMAN_FORMAT_LEN) { }

const char *CMySQLDateField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	char *p = (char *)buffer;
	bool is_null = false;

	CDate date_value = result_set->getDate(field, is_null);
	if (is_null) return nullptr;

	date_value.toStringGerman(p);
	return p;
}

const wchar_t *CMySQLDateField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	bool is_null = false;
	CDate date_value = result_set->getDate(field, is_null);
	if (is_null) return nullptr;

	date_value.toStringGerman(buffer);
	return buffer;
}

ImmutableString<char> CMySQLDateField::getValueAsImmutableString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	bool is_null = false;

	CDate date_value = result_set->getDate(field, is_null);
	if (is_null) return ImmutableString<char>(nullptr, 0);

	char *p = (char *)buffer;
	date_value.toStringGerman(p);
	return ImmutableString<char>(p, CDate::GERMAN_FORMAT_LEN);
}

ImmutableString<wchar_t> CMySQLDateField::getValueAsImmutableWString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	
	bool is_null = false;
	CDate date_value = result_set->getDate(field, is_null);
	if (is_null) return ImmutableString<wchar_t>(nullptr, 0);

	date_value.toStringGerman(buffer);
	return ImmutableString<wchar_t>(buffer, CDate::GERMAN_FORMAT_LEN);
}

void CMySQLDateField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const char *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.isNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const wchar_t *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.isNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
										std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no) const {

	bool is_null = false;
	CDate date_value = result_set->getDate(field, is_null);

	if (is_null)
		binding_target->bindNull(param_no);
	else
		binding_target->bindValue(param_no, date_value);
}

const char *CMySQLDateField::getQuoteStr() const {

	return "\"";
}

CMySQLDateField::~CMySQLDateField() { }

//**************************************************

CMySQLStringField::CMySQLStringField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_) : \
									CMySQLField(stmt_, field_) { }

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

const char *CMySQLStringField::getQuoteStr() const {

	return "\"";
}

CMySQLStringField::~CMySQLStringField() { }
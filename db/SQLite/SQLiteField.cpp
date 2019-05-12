#include "SQLiteField.h"

SQLiteFieldException::SQLiteFieldException(const int err_code, \
											const Tchar *err_descr) : \
											SQLiteException(err_code, err_descr) { }

SQLiteFieldException::SQLiteFieldException(sqlite3 *conn) : \
											SQLiteException(conn) { }

SQLiteFieldException::SQLiteFieldException(const SQLiteFieldException &obj) : \
											SQLiteException(obj) { }

SQLiteFieldException::~SQLiteFieldException() { }

//*************************************************************

SQLiteField::SQLiteField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, \
							const size_t field_, const char *field_type) : \
						stmt_handle(stmt_handle_), field(field_) {

	max_length = getFieldMaxLengthByType(field_type);
}

SQLiteField::SQLiteField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, \
							const size_t field_, const char *field_type, \
							const size_t def_max_length) : \
						stmt_handle(stmt_handle_), field(field_) {

	max_length = getFieldMaxLengthByType(field_type);
	if (max_length == (size_t)-1)
		max_length = def_max_length;
}

SQLiteField::SQLiteField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, \
							const size_t field_, const size_t def_max_length) : \
						stmt_handle(stmt_handle_), field(field_), \
						max_length(def_max_length) { }

std::string SQLiteField::getFieldName() const {

	const char *table_name = sqlite3_column_table_name(stmt_handle->stmt, (int)field);
	std::string buffer;
	auto stmt = getTableFieldMetaInfo(stmt_handle->conn, table_name, field, buffer);

	buffer = getFieldNameFromMetaInfo(stmt);
	sqlite3_finalize(stmt);

	return std::move(buffer);
}

std::wstring SQLiteField::getFieldNameW() const {

	auto table_name = (const wchar_t *)sqlite3_column_table_name16(stmt_handle->stmt, (int)field);
	std::wstring buffer;
	auto stmt = getTableFieldMetaInfo(stmt_handle->conn, table_name, field, buffer);

	buffer = getFieldNameWFromMetaInfo(stmt);
	sqlite3_finalize(stmt);

	return std::move(buffer);
}

size_t SQLiteField::getFieldMaxLength() const {

	return max_length;
}

std::string SQLiteField::getTableName() const {

	return std::string(sqlite3_column_table_name(stmt_handle->stmt, (int)field));
}

std::wstring SQLiteField::getTableNameW() const {

	return std::wstring((const wchar_t *)sqlite3_column_table_name16(stmt_handle->stmt, (int)field));
}

bool SQLiteField::isPrimaryKey() const {

	const char *table_name = sqlite3_column_table_name(stmt_handle->stmt, (int)field);
	std::string buffer;
	auto stmt = getTableFieldMetaInfo(stmt_handle->conn, table_name, field, buffer);

	bool is_primary = isFieldPrimKeyFromMetaInfo(stmt);
	sqlite3_finalize(stmt);

	return is_primary;
}

SQLiteField::~SQLiteField() { }

//**************************************************

SQLiteIntegerField::SQLiteIntegerField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, \
										const size_t field_, const char *field_type) : \
											SQLiteField(stmt_handle_, field_, \
														field_type, getDigitsCountOfType<int>()) { }

const char *SQLiteIntegerField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;

	return XConv::ToString(result_set->getInt(field), p);
}

const wchar_t *SQLiteIntegerField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return XConv::ToString(result_set->getInt(field), buffer);
}

ImmutableString<char> SQLiteIntegerField::getValueAsImmutableString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;
	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), p, size);
	return ImmutableString<char>(str, size);
}

ImmutableString<wchar_t> SQLiteIntegerField::getValueAsImmutableWString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), buffer, size);
	return ImmutableString<wchar_t>(str, size);
}

void SQLiteIntegerField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const char *value) const {

	int err = 0;
	int i = XConv::ToInt(value, err);

	if (err) {
		SQLiteFieldException e(CDbException::E_DB_WRONG_VALUE, \
								_T("the binding value is not an integer: "));
		e << value << _T(" , param no: ") << param_no;
		throw e;
	}

	binding_target->bindValue(param_no, i);
}

void SQLiteIntegerField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const wchar_t *value) const {

	int err = 0;
	int i = XConv::ToInt(value, err);

	if (err) {
		SQLiteFieldException e(CDbException::E_DB_WRONG_VALUE, \
								_T("the binding value is not an integer: "));
		e << value << _T(" , param no: ") << param_no;
		throw e;
	}

	binding_target->bindValue(param_no, i);
}

void SQLiteIntegerField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
											std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no) const {
	int i = result_set->getInt(field);
	binding_target->bindValue(param_no, i);
}

SQLiteIntegerField::~SQLiteIntegerField() { }

//**************************************************

SQLiteDateField::SQLiteDateField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, const size_t field_) : \
									SQLiteField(stmt_handle_, field, CDate::GERMAN_FORMAT_LEN) { }

const char *SQLiteDateField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return p;
}

const wchar_t *SQLiteDateField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return buffer;
}

ImmutableString<char> SQLiteDateField::getValueAsImmutableString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return ImmutableString<char>(p, CDate::GERMAN_FORMAT_LEN);
}

ImmutableString<wchar_t> SQLiteDateField::getValueAsImmutableWString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return ImmutableString<wchar_t>(buffer, CDate::GERMAN_FORMAT_LEN);
}

void SQLiteDateField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no, \
													const char *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void SQLiteDateField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no, \
													const wchar_t *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void SQLiteDateField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
													std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no) const {

	CDate date_value = result_set->getDate(field);
	binding_target->bindValue(param_no, date_value);
}

SQLiteDateField::~SQLiteDateField() { }

//**************************************************

SQLiteStringField::SQLiteStringField(std::shared_ptr<SQLiteStmtHandle> stmt_handle_, \
										const size_t field_, const char *field_type) : \
									SQLiteField(stmt_handle_, field, field_type) {
	
	assert(getFieldMaxLength() != (size_t)-1);
}

const char *SQLiteStringField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getString(field);
}

const wchar_t *SQLiteStringField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getWString(field);
}

ImmutableString<char> SQLiteStringField::getValueAsImmutableString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableString(field);
}

ImmutableString<wchar_t> SQLiteStringField::getValueAsImmutableWString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableWString(field);
}

void SQLiteStringField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no, \
													const char *value) const {

	assert(value);
	binding_target->bindValue(param_no, value);
}

void SQLiteStringField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no, \
													const wchar_t *value) const {

	assert(value);
	binding_target->bindValue(param_no, value);
}

void SQLiteStringField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
													std::shared_ptr<IDbBindingTarget> binding_target, \
													const size_t param_no) const {

	const char *value = result_set->getString(field);
	binding_target->bindValue(param_no, value);
}

SQLiteStringField::~SQLiteStringField() { }
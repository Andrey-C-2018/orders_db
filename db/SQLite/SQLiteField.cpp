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

SQLiteField::SQLiteField(std::shared_ptr<sqlite3> db_, \
							std::shared_ptr<Statement> stmt_, \
							const size_t field_, const char *field_type) : \
						db(db_), stmt(stmt_), field(field_) {

	max_length = getFieldMaxLengthByType(field_type);
}

SQLiteField::SQLiteField(std::shared_ptr<sqlite3> db_, \
							std::shared_ptr<Statement> stmt_, \
							const size_t field_, const char *field_type, \
							const size_t def_max_length) : \
						db(db_), stmt(stmt_), field(field_) {

	max_length = getFieldMaxLengthByType(field_type);
	if (max_length == (size_t)-1)
		max_length = def_max_length;
}

SQLiteField::SQLiteField(std::shared_ptr<sqlite3> db_, \
							std::shared_ptr<Statement> stmt_, \
							const size_t field_, const size_t def_max_length) : \
						db(db_), stmt(stmt_), field(field_), \
						max_length(def_max_length) { }

std::string SQLiteField::getFieldName() const {

	return std::string(sqlite3_column_origin_name(stmt->stmt, (int)field));
}

std::wstring SQLiteField::getFieldNameW() const {

	return std::wstring((const wchar_t *)sqlite3_column_origin_name16(stmt->stmt, (int)field));
}

std::string SQLiteField::getFieldAlias() const {

	return std::string(sqlite3_column_name(stmt->stmt, (int)field));
}

std::wstring SQLiteField::getFieldAliasW() const {

	return std::wstring((const wchar_t *)sqlite3_column_name16(stmt->stmt, (int)field));
}

size_t SQLiteField::getFieldMaxLength() const {

	return max_length;
}

std::string SQLiteField::getTableName() const {

	return std::string(sqlite3_column_table_name(stmt->stmt, (int)field));
}

std::wstring SQLiteField::getTableNameW() const {

	return std::wstring((const wchar_t *)sqlite3_column_table_name16(stmt->stmt, (int)field));
}

bool SQLiteField::isPrimaryKey() const {

	const char *table_name = sqlite3_column_table_name(stmt->stmt, (int)field);
	const char *field_name = sqlite3_column_origin_name(stmt->stmt, (int)field);

	std::string buffer;
	auto stmt = getTableFieldMetaInfo(db.get(), table_name, field_name, buffer);

	int rc = sqlite3_step(stmt);
	assert(rc != SQLITE_ERROR && rc != SQLITE_DONE);

	bool is_primary = isFieldPrimKeyFromMetaInfo(stmt);
	sqlite3_finalize(stmt);

	return is_primary;
}

SQLiteField::~SQLiteField() { }

//**************************************************

SQLiteIntegerField::SQLiteIntegerField(std::shared_ptr<sqlite3> db_, \
										std::shared_ptr<Statement> stmt_, \
										const size_t field_, const char *field_type) : \
											SQLiteField(db_, stmt_, field_, \
														field_type, getDigitsCountOfType<int>()) { }

const char *SQLiteIntegerField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;
	bool is_null = false;
	int int_value = result_set->getInt(field, is_null);

	return is_null ? nullptr : XConv::ToString(int_value, p);
}

const wchar_t *SQLiteIntegerField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	bool is_null = false;
	int int_value = result_set->getInt(field, is_null);

	return is_null ? nullptr : XConv::ToString(int_value, buffer);
}

ImmutableString<char> SQLiteIntegerField::getValueAsImmutableString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;
	size_t size = 0;

	bool is_null = false;
	int int_value = result_set->getInt(field, is_null);

	const char *str = is_null ? nullptr : XConv::ToString(int_value, p, size);
	return ImmutableString<char>(str, size);
}

ImmutableString<wchar_t> SQLiteIntegerField::getValueAsImmutableWString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	size_t size = 0;

	bool is_null = false;
	int int_value = result_set->getInt(field, is_null);

	const wchar_t *str = is_null ? nullptr : XConv::ToString(int_value, buffer, size);
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
	bool is_null = false;
	int i = result_set->getInt(field, is_null);

	if (is_null)
		binding_target->bindNull(param_no);
	else
		binding_target->bindValue(param_no, i);
}

SQLiteIntegerField::~SQLiteIntegerField() { }

//**************************************************

SQLiteDateField::SQLiteDateField(std::shared_ptr<sqlite3> db_, \
									std::shared_ptr<Statement> stmt_, const size_t field_) : \
									SQLiteField(db_, stmt_, field_, CDate::GERMAN_FORMAT_LEN) { }

const char *SQLiteDateField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;

	bool is_null;
	CDate date_value = result_set->getDate(field, is_null);
	date_value.ToStringGerman(p);

	return p;
}

const wchar_t *SQLiteDateField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	bool is_null;
	CDate date_value = result_set->getDate(field, is_null);
	date_value.ToStringGerman(buffer);

	return buffer;
}

ImmutableString<char> SQLiteDateField::getValueAsImmutableString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {

	char *p = (char *)buffer;

	bool is_null;
	CDate date_value = result_set->getDate(field, is_null);
	date_value.ToStringGerman(p);

	return ImmutableString<char>(p, CDate::GERMAN_FORMAT_LEN);
}

ImmutableString<wchar_t> SQLiteDateField::getValueAsImmutableWString(\
													const std::shared_ptr<const IDbResultSet> result_set) const {
	bool is_null;
	CDate date_value = result_set->getDate(field, is_null);
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

	bool is_null;
	CDate date_value = result_set->getDate(field, is_null);

	if (is_null)
		binding_target->bindNull(param_no);
	else
		binding_target->bindValue(param_no, date_value);
}

SQLiteDateField::~SQLiteDateField() { }

//**************************************************

SQLiteStringField::SQLiteStringField(std::shared_ptr<sqlite3> db_, \
										std::shared_ptr<Statement> stmt_, \
										const size_t field_, const char *field_type) : \
									SQLiteField(db_, stmt_, field_, field_type) {
	
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
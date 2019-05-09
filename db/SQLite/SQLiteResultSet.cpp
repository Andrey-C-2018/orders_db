#include "SQLiteResultSet.h"
#include "../IDbBindingTarget.h"

SQLiteResultSetException::SQLiteResultSetException(const int err_code, \
									const Tchar *err_descr) : \
									SQLiteException(err_code, err_descr) { }

SQLiteResultSetException::SQLiteResultSetException(sqlite3 *conn) : \
													SQLiteException(conn) { }

SQLiteResultSetException::SQLiteResultSetException(const SQLiteResultSetException &obj) : \
													SQLiteException(obj) { }

SQLiteResultSetException::~SQLiteResultSetException() { }

//***************************************************************

SQLiteResultSet::SQLiteResultSet(std::shared_ptr<SQLiteStmtHandle> stmt_handle_) : \
								fields_count(0), records_count(0), \
								stmt_handle(stmt_handle), \
								eof(false), fetched_record_count(0) {
	assert(stmt_handle_);

	fields_count = sqlite3_column_count(stmt_handle->stmt);
	records_count = 0;
}

SQLiteResultSet::SQLiteResultSet(SQLiteResultSet &&obj) : \
								fields_count(obj.fields_count), \
								records_count(obj.records_count), \
								stmt_handle(std::move(obj.stmt_handle)), \
								eof(obj.eof), \
								fetched_record_count(obj.fetched_record_count) {

	assert(fields_count || !records_count);
	obj.fields_count = 0;
	obj.records_count = 0;
	obj.eof = false;
	obj.fetched_record_count = 0;
}

SQLiteResultSet &SQLiteResultSet::operator=(SQLiteResultSet &&obj) {

	assert(fields_count || !records_count);
	
	fields_count = obj.fields_count;
	records_count = obj.records_count;
	stmt_handle = std::move(obj.stmt_handle);
	eof = obj.eof;
	fetched_record_count = obj.fetched_record_count;
	
	obj.records_count = 0;
	obj.fields_count = 0;
	obj.eof = false;
	obj.fetched_record_count = 0;
	
	return *this;
}

bool SQLiteResultSet::empty() const {

	return !(records_count && fields_count);
}

size_t SQLiteResultSet::getFieldsCount() const {

	return fields_count;
}

size_t SQLiteResultSet::getRecordsCount() const {

	return records_count;
}

void SQLiteResultSet::gotoRecord(const size_t record) const {

	assert(record < records_count);
	assert(stmt_handle->stmt);

	if (!eof && record + 1 == fetched_record_count) return;
	if (eof || record + 1 < fetched_record_count) {
		sqlite3_reset(stmt_handle->stmt);
		fetched_record_count = 0;
		eof = false;
	}

	int rc;
	bool Key = !eof;
	while (Key && (rc = sqlite3_step(stmt_handle->stmt)) != SQLITE_DONE) {
		Key = (rc != SQLITE_ERROR) && (fetched_record_count < record);
		fetched_record_count++;
	}

	if (Key && rc == SQLITE_ERROR)
		throw SQLiteResultSetException(stmt_handle->conn);
	
	eof = (rc == SQLITE_DONE);
}

int SQLiteResultSet::getInt(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);
	return sqlite3_column_int(stmt_handle->stmt, (int)field);
}

const char *SQLiteResultSet::getString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);
	return (const char *)sqlite3_column_text(stmt_handle->stmt, (int)field);
}

const wchar_t *SQLiteResultSet::getWString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);
	return (const wchar_t *)sqlite3_column_text16(stmt_handle->stmt, (int)field);
}

ImmutableString<char> SQLiteResultSet::getImmutableString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);

	const char *value = (const char *)sqlite3_column_text(stmt_handle->stmt, (int)field);
	size_t len = strlen(value);

	return ImmutableString<char>(value, len);
}

ImmutableString<wchar_t> SQLiteResultSet::getImmutableWString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);

	const wchar_t *value = (const wchar_t *)sqlite3_column_text16(stmt_handle->stmt, (int)field);
	size_t len = wcslen(value);

	return ImmutableString<wchar_t>(value, len);
}

CDate SQLiteResultSet::getDate(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_count != 0);
	const char *value = (const char *)sqlite3_column_text(stmt_handle->stmt, (int)field);

	return CDate(value, CDate::SQL_FORMAT);
}

void SQLiteResultSet::Release() {

	stmt_handle.reset();
}

SQLiteResultSet::~SQLiteResultSet() {

	Release();
}
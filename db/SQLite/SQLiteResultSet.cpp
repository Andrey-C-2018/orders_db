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

SQLiteResultSet::SQLiteResultSet(std::shared_ptr<sqlite3> db_, \
								std::shared_ptr<Statement> stmt_) : \
							fields_count(0), records_count(0), \
							db(db_), stmt(stmt_), \
							eof(false), fetched_record_no(0) {
	assert(db);
	assert(stmt);
	assert(stmt->stmt);
	assert(stmt->stmt_records_count);
	assert(sqlite3_column_count(stmt->stmt_records_count) == 1);

	fields_count = sqlite3_column_count(stmt->stmt);

	reloadInternal();
}

SQLiteResultSet::SQLiteResultSet(SQLiteResultSet &&obj) noexcept : \
								fields_count(obj.fields_count), \
								records_count(obj.records_count), \
								db(std::move(obj.db)), \
								stmt(std::move(obj.stmt)), \
								eof(obj.eof), \
								fetched_record_no(obj.fetched_record_no) {

	assert(fields_count || !records_count);
	obj.fields_count = 0;
	obj.records_count = 0;
	obj.eof = false;
	obj.fetched_record_no = 0;
}

SQLiteResultSet &SQLiteResultSet::operator=(SQLiteResultSet &&obj) noexcept {

	assert(fields_count || !records_count);
	
	fields_count = obj.fields_count;
	records_count = obj.records_count;
	db = std::move(obj.db);
	stmt = std::move(obj.stmt);
	eof = obj.eof;
	fetched_record_no = obj.fetched_record_no;
	
	obj.records_count = 0;
	obj.fields_count = 0;
	obj.eof = false;
	obj.fetched_record_no = 0;
	
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
	assert(stmt->stmt);

	if (!eof && record + 1 == fetched_record_no) return;
	if (eof || record + 1 < fetched_record_no) {
		sqlite3_reset(stmt->stmt);
		stmt->initial_state = true;

		fetched_record_no = 0;
		eof = false;
	}

	int rc;
	bool Key = !eof;
	while (Key && (rc = execute_stmt()) != SQLITE_DONE) {
		Key = (rc != SQLITE_ERROR) && (fetched_record_no < record);
		fetched_record_no++;
	}

	if (Key && rc == SQLITE_ERROR)
		throw SQLiteResultSetException(db.get());
	
	eof = (rc == SQLITE_DONE);
}

int SQLiteResultSet::getInt(const size_t field, bool &is_null) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);
	// is_null ???
	is_null = false;
	return sqlite3_column_int(stmt->stmt, (int)field);
}

const char *SQLiteResultSet::getString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);
	return (const char *)sqlite3_column_text(stmt->stmt, (int)field);
}

const wchar_t *SQLiteResultSet::getWString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);
	return (const wchar_t *)sqlite3_column_text16(stmt->stmt, (int)field);
}

ImmutableString<char> SQLiteResultSet::getImmutableString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);

	const char *value = (const char *)sqlite3_column_text(stmt->stmt, (int)field);
	size_t len = strlen(value);

	return ImmutableString<char>(value, len);
}

ImmutableString<wchar_t> SQLiteResultSet::getImmutableWString(const size_t field) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);

	auto value = (const wchar_t *)sqlite3_column_text16(stmt->stmt, (int)field);
	size_t len = wcslen(value);

	return ImmutableString<wchar_t>(value, len);
}

CDate SQLiteResultSet::getDate(const size_t field, bool &is_null) const {

	assert(field < fields_count);
	assert(fetched_record_no != 0);
	const char *value = (const char *)sqlite3_column_text(stmt->stmt, (int)field);

	is_null = (value == nullptr);
	return CDate(value, CDate::SQL_FORMAT);
}

void SQLiteResultSet::reload() {

	reloadInternal();
}

void SQLiteResultSet::Release() {

	fields_count = records_count = fetched_record_no = 0;
	
	stmt.reset();
	db.reset();
}

std::shared_ptr<IDbResultSet> SQLiteResultSet::staticClone() const {

	throw XException(0, "staticClone() is not supported");
}

SQLiteResultSet::~SQLiteResultSet() {

	Release();
}

#include "SQLiteResultSet.h"
#include "../IDbBindingTarget.h"

SQLiteResultSet::CMySQLResultSet(std::shared_ptr<SQLiteStmtHandle> stmt_handle_) : \
								fields_count(0), records_count(0), \
								stmt_handle(stmt_handle), \
								curr_record((size_t)-1) {
	assert(stmt);

	fields_count = sqlite3_column_count(stmt_handle->stmt);
	records_count = 0;
}

SQLiteResultSet::SQLiteResultSet(SQLiteResultSet &&obj) : \
								fields_count(obj.fields_count), \
								records_count(obj.records_count), \
								stmt(std::move(obj.stmt)), \
								curr_record(obj.curr_record) {

	assert(fields_count || !records_count);
	obj.fields_count = 0;
	obj.records_count = 0;
	obj.curr_record = (size_t)-1;
}

SQLiteResultSet &SQLiteResultSet::operator=(SQLiteResultSet &&obj) {

	assert(fields_count || !records_count);
	
	fields_count = obj.fields_count;
	records_count = obj.records_count;
	stmt = std::move(obj.stmt);
	curr_record = obj.curr_record;
	
	obj.records_count = 0;
	obj.fields_count = 0;
	obj.curr_record = (size_t)-1;
	
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
	if (curr_record == record) return;

	assert(stmt);
	// TODO: implement

	curr_record = record;
}

int SQLiteResultSet::getInt(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	return sqlite3_column_int(stmt, field);
}

const char *SQLiteResultSet::getString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	return sqlite3_column_text(stmt, field);
}

const wchar_t *SQLiteResultSet::getWString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	return sqlite3_column_text16(stmt, field);
}

ImmutableString<char> SQLiteResultSet::getImmutableString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);

	const char *value = sqlite3_column_text(stmt, field);
	size_t len = strlen(value);

	return ImmutableString<char>(str, len);
}

ImmutableString<wchar_t> SQLiteResultSet::getImmutableWString(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);

	const wchar_t *value = sqlite3_column_text16(stmt, field);
	size_t len = wcslen(value);

	return ImmutableString<wchar_t>(str, len);
}

CDate SQLiteResultSet::getDate(const size_t field) const {

	assert(field < fields_count);
	assert(curr_record != (size_t)-1);
	const char *value = sqlite3_column_text(stmt, field);

	return CDate();
}

void SQLiteResultSet::Release() {

	stmt.reset();
}

SQLiteResultSet::~SQLiteResultSet() {

	Release();
}
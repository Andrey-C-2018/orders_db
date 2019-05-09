#include "SQLiteResultSetMetadata.h"
#include "SQLiteField.h"

SQLiteResultSetMetadata::SQLiteResultSetMetadata(std::shared_ptr<SQLiteStmtHandle> stmt_handle_) : \
												stmt_handle(stmt_handle_) {

	assert(stmt_handle_);
	fields_count = sqlite3_column_count(stmt_handle->stmt);
}

size_t SQLiteResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

std::shared_ptr<IDbField> SQLiteResultSetMetadata::getField(const size_t field) const {

	assert(field < fields_count);
	int column_type = sqlite3_column_type(stmt_handle->stmt, (int)field);
	const char *column_origin = sqlite3_column_origin_name(stmt_handle->stmt, (int)field);
	
	if (column_type == SQLITE_TEXT) {
		if (strstr(column_origin, "date"))
			return std::make_shared<SQLiteDateField>(stmt_handle, field);
		else
			return std::make_shared<SQLiteStringField>(stmt_handle, field);
	}

	if (column_type == SQLITE_TEXT)
		return std::make_shared<SQLiteIntegerField>(stmt_handle, field);

	XException e(0, _T("No such field type: "));
	e << column_type << ", column origin: " << column_origin;
	throw e;

	assert(false);
	return std::shared_ptr<IDbField>();
}

SQLiteResultSetMetadata::~SQLiteResultSetMetadata() { }

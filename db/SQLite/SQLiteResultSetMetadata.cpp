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
	const char *column_table = sqlite3_column_table_name(stmt_handle->stmt, (int)field);

	std::string query;
	sqlite3_stmt *stmt = getTableFieldMetaInfo(stmt_handle->conn, column_table, field, query);
	if (!stmt) {
		SQLiteException e(0, _T("Cannot get a meta-info of the column: '"));
		e << field << "' of the table '" << column_table;
		throw e;
	}
	
	int rc = sqlite3_step(stmt);
	assert(rc != SQLITE_ERROR && rc != SQLITE_DONE);
	assert(!strcmp(getFieldNameFromMetaInfo(stmt), \
			sqlite3_column_name(stmt_handle->stmt, (int)field)));

	query = (const char *) sqlite3_column_text(stmt, 2);
	auto column_type_str = query.c_str();
	
	sqlite3_finalize(stmt);

	if(column_type == SQLITE_TEXT){
		if (!strcmp(column_type_str, "DATE")) 
			return std::make_shared<SQLiteDateField>(stmt_handle, field);
		else
			return std::make_shared<SQLiteStringField>(stmt_handle, field, column_type_str);
	}

	if (column_type == SQLITE_INTEGER)
		return std::make_shared<SQLiteIntegerField>(stmt_handle, field, column_type_str);

	SQLiteException e(0, _T("No such field type, type ID: "));
	e << column_type << ", type name: " << column_type_str;
	e << ", table_name: " << column_table;
	throw e;

	return std::shared_ptr<IDbField>();
}

SQLiteResultSetMetadata::~SQLiteResultSetMetadata() { }

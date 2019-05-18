#include "SQLiteResultSetMetadata.h"
#include "SQLiteField.h"

SQLiteResultSetMetadata::SQLiteResultSetMetadata(std::shared_ptr<sqlite3> db_, \
													std::shared_ptr<Statement> stmt_) : \
												db(db_), stmt(stmt_) {

	assert(db);
	assert(stmt);
	assert(stmt->stmt);

	fields_count = sqlite3_column_count(stmt->stmt);
}

size_t SQLiteResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

std::shared_ptr<IDbField> SQLiteResultSetMetadata::getField(const size_t field) const {

	assert(field < fields_count);
	const char *column_table = sqlite3_column_table_name(stmt->stmt, (int)field);
	const char *column_name = sqlite3_column_origin_name(stmt->stmt, (int)field);

	std::string query;
	sqlite3_stmt *stmt = getTableFieldMetaInfo(db.get(), column_table, column_name, query);
	if (!stmt) {
		SQLiteException e(0, _T("Cannot get a meta-info of the column: '"));
		e << field << "' of the table '" << column_table;
		throw e;
	}
	
	int rc = sqlite3_step(stmt);
	assert(rc != SQLITE_ERROR && rc != SQLITE_DONE);
	assert(!strcmp(getFieldNameFromMetaInfo(stmt), column_name));

	query = (const char *) sqlite3_column_text(stmt, 2);
	auto column_type_str = query.c_str();
	
	sqlite3_finalize(stmt);

	if (!strcmp(column_type_str, "DATE")) 
		return std::make_shared<SQLiteDateField>(db, this->stmt, field);
	
	if (strstr(column_type_str, "CHAR") || strstr(column_type_str, "TEXT") || \
		strstr(column_type_str, "DECIMAL"))
			return std::make_shared<SQLiteStringField>(db, this->stmt, field, column_type_str);

	if (strstr(column_type_str, "INT") || !strcmp(column_type_str, "INTEGER"))
		return std::make_shared<SQLiteIntegerField>(db, this->stmt, field, column_type_str);

	SQLiteException e(0, _T("No such field type: "));
	e << column_type_str;
	e << ", table_name: " << column_table;
	throw e;

	return std::shared_ptr<IDbField>();
}

SQLiteResultSetMetadata::~SQLiteResultSetMetadata() { }

#include "SQLiteStatement.h"
#include "SQLiteResultSet.h"

SQLiteStatementException::SQLiteStatementException(const int err_code, \
												const Tchar *err_descr) : \
												SQLiteException(err_code, err_descr) { }

SQLiteStatementException::SQLiteStatementException(sqlite3 *conn) : \
												SQLiteException(conn) { }

SQLiteStatementException::SQLiteStatementException(const SQLiteStatementException &obj) : \
												SQLiteException(obj) { }

SQLiteStatementException::~SQLiteStatementException() { }

//***************************************************************

SQLiteStatement::SQLiteStatement(SQLiteStmtHandle stmt_handle) {

	assert(stmt_handle.conn);
	assert(stmt_handle.stmt);

	stmt = std::shared_ptr<SQLiteStmtHandle>(new SQLiteStmtHandle(stmt_handle), \
		[](SQLiteStmtHandle *stmt_handle) { 
		
			int err = sqlite3_finalize(stmt_handle->stmt);
			assert(err == SQLITE_OK);
		});

	params_count = sqlite3_bind_parameter_count(stmt->stmt);
}

SQLiteStatement::SQLiteStatement(SQLiteStatement &&obj) : stmt(std::move(obj.stmt)) { }

SQLiteStatement &SQLiteStatement::operator=(SQLiteStatement &&obj) {

	stmt = std::move(obj.stmt);
	return *this;
}

void SQLiteStatement::bindValue(const size_t param_no, const int value) {

	assert(param_no < params_count);
	sqlite3_bind_int(stmt->stmt, param_no, value);
}

void SQLiteStatement::bindValue(const size_t param_no, const char *value) {

	assert(param_no < params_count);
	sqlite3_bind_text(stmt->stmt, param_no, value, -1, nullptr);
}

void SQLiteStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert(param_no < params_count);
	sqlite3_bind_text16(stmt->stmt, param_no, value, -1, nullptr);
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	assert(param_no < params_count);
	sqlite3_bind_text(stmt->stmt, param_no, value.str, value.size, nullptr);
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	assert(param_no < params_count);
	sqlite3_bind_text16(stmt->stmt, param_no, value.str, value.size, nullptr);
}

void SQLiteStatement::bindValue(const size_t param_no, const CDate &value) {

	assert(param_no < params_count);

	char date_str[CDate::SQL_FORMAT_LEN + 1];
	value.ToStringSQL(date_str);

	sqlite3_bind_text(stmt->stmt, param_no, date_str, CDate::SQL_FORMAT_LEN, nullptr);
}

void SQLiteStatement::bindNull(const size_t param_no) {

	assert(param_no < params_count);
	sqlite3_bind_null(stmt->stmt, param_no);
}

std::shared_ptr<IDbResultSet> SQLiteStatement::exec() {

	if(!sqlite3_column_count(stmt->stmt))
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
								_T("exec() used instead of execScalar()"));

	int err = sqlite3_step(stmt->stmt);
	if (err == SQLITE_ERROR)
		throw SQLiteStatementException(stmt->conn);

	return std::make_shared<SQLiteResultSet>(stmt);
}

record_t SQLiteStatement::execScalar() {

	if (sqlite3_column_count(stmt->stmt))
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
			_T("execScalar() used instead of exec()"));

	int err = sqlite3_step(stmt->stmt);
	if (err == SQLITE_ERROR)
		throw SQLiteStatementException(stmt->conn);

	return 1;
}

std::shared_ptr<IDbResultSetMetadata> SQLiteStatement::getResultSetMetadata() {

	return std::make_shared<SQLiteResultSetMetadata>(stmt);
}

SQLiteStatement::~SQLiteStatement() { }
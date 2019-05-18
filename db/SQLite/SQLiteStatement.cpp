#include "SQLiteStatement.h"
#include "SQLiteResultSet.h"
#include "SQLiteResultSetMetadata.h"

SQLiteStatementException::SQLiteStatementException(const int err_code, \
												const Tchar *err_descr) : \
												SQLiteException(err_code, err_descr) { }

SQLiteStatementException::SQLiteStatementException(sqlite3 *conn) : \
												SQLiteException(conn) { }

SQLiteStatementException::SQLiteStatementException(const SQLiteStatementException &obj) : \
												SQLiteException(obj) { }

SQLiteStatementException::~SQLiteStatementException() { }

//***************************************************************

SQLiteStatement::SQLiteStatement(std::shared_ptr<sqlite3> db_, const char *query_text) : db(db_) {

	assert(db);
	assert(query_text);

	auto main_stmt = prepareQuery(db.get(), query_text);
	if (!main_stmt) throw SQLiteStatementException(db.get());

	sqlite3_stmt *rec_count_stmt = nullptr;
	if (sqlite3_column_count(main_stmt)) {
		std::string rec_count_query = "SELECT COUNT(*) FROM (";
		rec_count_query += query_text;
		rec_count_query += ") a1b2c3";

		rec_count_stmt = prepareQuery(db.get(), rec_count_query.c_str());
		if (!rec_count_stmt) {
			int err = sqlite3_finalize(main_stmt);
			assert(err == SQLITE_OK);
			throw SQLiteStatementException(db.get());
		}
	}

	stmt = std::shared_ptr<Statement>(new Statement{ main_stmt, rec_count_stmt }, \
		[](Statement *stmts) {
		
			int err = sqlite3_finalize(stmts->stmt);
			assert(err == SQLITE_OK);

			if (stmts->stmt_records_count) {
				err = sqlite3_finalize(stmts->stmt_records_count);
				assert(err == SQLITE_OK);
			}

			delete stmts;
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
	int rc = sqlite3_bind_int(stmt->stmt, (int)param_no + 1, value);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindValue(const size_t param_no, const char *value) {

	assert(param_no < params_count);
	int rc = sqlite3_bind_text(stmt->stmt, (int)param_no + 1, value, -1, nullptr);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert(param_no < params_count);
	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, value, -1, nullptr);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	assert(param_no < params_count);
	int rc = sqlite3_bind_text(stmt->stmt, (int)param_no + 1, value.str, (int)value.size, nullptr);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	assert(param_no < params_count);
	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, \
									value.str, (int)(value.size * sizeof(wchar_t)), nullptr);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindValue(const size_t param_no, const CDate &value) {

	assert(param_no < params_count);

	wchar_t date_str[CDate::SQL_FORMAT_LEN + 1];
	value.ToStringSQL(date_str);

	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, \
									date_str, CDate::SQL_FORMAT_LEN * sizeof(wchar_t), nullptr);
	assert(rc != SQLITE_ERROR);
}

void SQLiteStatement::bindNull(const size_t param_no) {

	assert(param_no < params_count);
	int rc = sqlite3_bind_null(stmt->stmt, (int)param_no + 1);
	assert(rc != SQLITE_ERROR);
}

std::shared_ptr<IDbResultSet> SQLiteStatement::exec() {

	if(!sqlite3_column_count(stmt->stmt))
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
								_T("exec() used instead of execScalar()"));

	return std::make_shared<SQLiteResultSet>(db, stmt);
}

record_t SQLiteStatement::execScalar() {

	if (sqlite3_column_count(stmt->stmt))
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
								_T("execScalar() used instead of exec()"));

	if (sqlite3_step(stmt->stmt) == SQLITE_ERROR)
		throw SQLiteStatementException(db.get());

	return sqlite3_changes(db.get());
}

std::shared_ptr<IDbResultSetMetadata> SQLiteStatement::getResultSetMetadata() {

	return std::make_shared<SQLiteResultSetMetadata>(db, stmt);
}

SQLiteStatement::~SQLiteStatement() { }
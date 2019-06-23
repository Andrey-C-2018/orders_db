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

SQLiteStatement::SQLiteStatement(std::shared_ptr<sqlite3> db_, const char *query_text) : \
									db(db_), fields_count(0), params_count(0) {

	assert(db);
	assert(query_text);

	auto main_stmt = prepareQuery(db.get(), query_text);
	if (!main_stmt) throw SQLiteStatementException(db.get());

	sqlite3_stmt *rec_count_stmt = nullptr;
	fields_count = sqlite3_column_count(main_stmt);
	if (fields_count) {
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

	stmt = std::make_shared<Statement>(main_stmt, rec_count_stmt, true);

	params_count = sqlite3_bind_parameter_count(stmt->stmt);
}

SQLiteStatement::SQLiteStatement(SQLiteStatement &&obj) : \
									stmt(std::move(obj.stmt)), \
										fields_count(obj.fields_count), \
										params_count(obj.params_count) {

	obj.fields_count = 0;
	obj.params_count = 0;
}

SQLiteStatement &SQLiteStatement::operator=(SQLiteStatement &&obj) {

	stmt = std::move(obj.stmt);
	fields_count = obj.fields_count;
	params_count = obj.params_count;

	obj.fields_count = 0;
	obj.params_count = 0;
	return *this;
}

void SQLiteStatement::bindValue(const size_t param_no, const int value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_int(stmt->stmt, (int)param_no + 1, value);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_int(stmt->stmt_records_count, (int)param_no + 1, value);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindValue(const size_t param_no, const char *value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_text(stmt->stmt, (int)param_no + 1, value, -1, nullptr);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_text(stmt->stmt_records_count, (int)param_no + 1, value, -1, nullptr);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, value, -1, nullptr);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_text16(stmt->stmt_records_count, (int)param_no + 1, value, -1, nullptr);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_text(stmt->stmt, (int)param_no + 1, value.str, (int)value.size, nullptr);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_text(stmt->stmt_records_count, (int)param_no + 1, value.str, (int)value.size, nullptr);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, \
									value.str, (int)(value.size * sizeof(wchar_t)), nullptr);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_text16(stmt->stmt_records_count, (int)param_no + 1, \
			value.str, (int)(value.size * sizeof(wchar_t)), nullptr);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindValue(const size_t param_no, const CDate &value) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	wchar_t date_str[CDate::SQL_FORMAT_LEN + 1];
	value.toStringSQL(date_str);

	int rc = sqlite3_bind_text16(stmt->stmt, (int)param_no + 1, \
									date_str, CDate::SQL_FORMAT_LEN * sizeof(wchar_t), nullptr);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_text16(stmt->stmt_records_count, (int)param_no + 1, \
			date_str, CDate::SQL_FORMAT_LEN * sizeof(wchar_t), nullptr);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

void SQLiteStatement::bindNull(const size_t param_no) {

	assert(param_no < params_count);
	assert(stmt->stmt);
	if (!stmt->initial_state) sqlite3_reset(stmt->stmt);

	int rc = sqlite3_bind_null(stmt->stmt, (int)param_no + 1);
	assert(rc != SQLITE_ERROR);

	if (fields_count) {
		if (!stmt->initial_state) sqlite3_reset(stmt->stmt_records_count);

		int rc = sqlite3_bind_null(stmt->stmt_records_count, (int)param_no + 1);
		assert(rc != SQLITE_ERROR);
	}
	stmt->initial_state = true;
}

std::shared_ptr<IDbResultSet> SQLiteStatement::exec() {

	if(!fields_count)
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
								_T("exec() used instead of execScalar()"));

	return std::make_shared<SQLiteResultSet>(db, stmt);
}

record_t SQLiteStatement::execScalar() {

	if (fields_count)
		throw SQLiteStatementException(SQLiteStatementException::E_EXEC, \
								_T("execScalar() used instead of exec()"));

	if (sqlite3_step(stmt->stmt) != SQLITE_DONE)
		throw SQLiteStatementException(db.get());

	stmt->initial_state = false;
	return sqlite3_changes(db.get());
}

std::shared_ptr<IDbResultSetMetadata> SQLiteStatement::getResultSetMetadata() {

	return std::make_shared<SQLiteResultSetMetadata>(db, stmt);
}

SQLiteStatement::~SQLiteStatement() { }
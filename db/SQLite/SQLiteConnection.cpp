#include "SQLiteConnection.h"
#include <string>
#include <assert.h>
#include "SQLiteStatement.h"

SQLiteConnectionException::SQLiteConnectionException(const int err_code, \
										const Tchar *err_descr) : \
										SQLiteException(err_code, err_descr) { }

SQLiteConnectionException::SQLiteConnectionException(sqlite3 *conn) : \
										SQLiteException(conn) { }

SQLiteConnectionException::SQLiteConnectionException(const SQLiteConnectionException &obj) : \
										SQLiteException(obj) { }

SQLiteConnectionException::~SQLiteConnectionException() { }

//***************************************************************

SQLiteConnection::SQLiteConnection() : db(nullptr) { }

SQLiteConnection::SQLiteConnection(SQLiteConnection &&obj) : db(obj.db) {

	obj.db = nullptr;
}

SQLiteConnection &SQLiteConnection::operator=(SQLiteConnection &&obj) {

	db = obj.db;
	obj.db = nullptr;
	return *this;
}

void SQLiteConnection::Connect(const char *location, const unsigned port, \
								const char *login, \
								const char *pwd, \
								const char *schema_name) {

	assert(location && schema_name);
	Disconnect();

	int err = 0;
	std::string full_path;

	full_path.reserve(strlen(location) + strlen(schema_name) + 4);
	full_path = location;

	size_t size = full_path.size();
	if (size && full_path[size - 1] != '\\')
		full_path += '\\';

	full_path += schema_name;
	full_path += ".db";
	err = sqlite3_open(full_path.c_str(), &db);
	if (err) 
		throw SQLiteConnectionException(db);
}

void SQLiteConnection::SetSchema(const char *schema_name) { }

sqlite3_stmt *SQLiteConnection::InternalPrepareQuery(const char *query_text) const {
	int err = 0;
	const int QUERY_IS_NULL_TERM_STR = -1;
	sqlite3_stmt *pStmt = nullptr;
	
	assert(db);
	err = sqlite3_prepare_v2(db, query_text, QUERY_IS_NULL_TERM_STR, &pStmt, NULL);
	if (err != SQLITE_OK)
		throw SQLiteConnectionException(db);

	return pStmt;
}

record_t SQLiteConnection::ExecScalarQuery(const char *query_text) {
	
	sqlite3_stmt *pStmt = InternalPrepareQuery(query_text);

	int err = 0;
	record_t records_count = 0;

	err = sqlite3_step(pStmt);
	if (err == SQLITE_ERROR) 
		throw SQLiteConnectionException(db);

	records_count = sqlite3_changes(db);

	sqlite3_finalize(pStmt);
	return records_count;
}

std::shared_ptr<IDbResultSet> SQLiteConnection::ExecQuery(const char *query_text) const {
	
	assert(query_text);

	auto stmt = InternalPrepareQuery(query_text);
	SQLiteStmtHandle stmt_handle{ db, stmt };
	SQLiteStatement sqlite_stmt(stmt_handle);

	if (sqlite_stmt.getParamsCount() > 0)
		throw SQLiteConnectionException(SQLiteConnectionException::E_WRONG_QUERY, \
			_T("Parameters are forbidden in the query text passed to ExecQuery()"));

	return sqlite_stmt.exec();
}

std::shared_ptr<IDbStatement> SQLiteConnection::PrepareQuery(const char *query_text) const {
	
	sqlite3_stmt *stmt = InternalPrepareQuery(query_text);
	SQLiteStmtHandle stmt_handle{ db, stmt };

	return std::make_shared<SQLiteStatement>(stmt_handle);
}

void SQLiteConnection::Disconnect() {

	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}
}

SQLiteConnection::~SQLiteConnection(){

	Disconnect();
}
#include "SQLiteConnection.h"
#include <string>
#include <chrono>
#include <thread>
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

SQLiteConnection::SQLiteConnection() { }

void SQLiteConnection::Connect(const char *location, const unsigned port, \
								const char *login, \
								const char *pwd, \
								const char *schema_name) {

	if (!location || !schema_name)
		throw SQLiteConnectionException(SQLiteConnectionException::E_CONNECTION_PARAMS, \
										_T("the path to database is NULL"));

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

	sqlite3 *db = nullptr;
	err = sqlite3_open(full_path.c_str(), &db);
	if (err) 
		throw SQLiteConnectionException(db);

	this->db = std::shared_ptr<sqlite3>(db, \
		[](sqlite3 *db) {
			int err = 0;

#ifdef SQLITE_DB_OPTIMIZEON_EXIT
			auto opt_stmt = prepareQuery(db, "PRAGMA optimize");
			assert(opt_stmt);
			err = sqlite3_step(opt_stmt);
			assert(err != SQLITE_ERROR);
#endif

			size_t i = 0;
			while (i < 4 && (err = sqlite3_close(db)) != SQLITE_OK) {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				++i;
			}
		});
}

void SQLiteConnection::SetSchema(const char *schema_name) { }

record_t SQLiteConnection::ExecScalarQuery(const char *query_text) {
	
	sqlite3_stmt *pStmt =prepareQuery(db.get(), query_text);
	if(!pStmt) throw SQLiteConnectionException(db.get());

	if (sqlite3_column_count(pStmt)) {
		SQLiteConnectionException e(SQLiteConnectionException::E_NOT_A_SCALAR_QUERY, \
									_T("The query id not CREATE/UPDATE/DELETE: '"));
		e << query_text << "'";
		throw e;
	}

	int err = 0;
	record_t records_count = 0;

	err = sqlite3_step(pStmt);
	if (err == SQLITE_ERROR) 
		throw SQLiteConnectionException(db.get());

	records_count = sqlite3_changes(db.get());

	sqlite3_finalize(pStmt);
	return records_count;
}

std::shared_ptr<IDbResultSet> SQLiteConnection::ExecQuery(const char *query_text) const {
	
	assert(query_text);

	SQLiteStatement sqlite_stmt(db, query_text);

	if (sqlite_stmt.getParamsCount() > 0)
		throw SQLiteConnectionException(SQLiteConnectionException::E_WRONG_QUERY, \
			_T("Parameters are forbidden in the query text passed to ExecQuery()"));

	return sqlite_stmt.exec();
}

std::shared_ptr<IDbStatement> SQLiteConnection::PrepareQuery(const char *query_text) const {
	
	return std::make_shared<SQLiteStatement>(db, query_text);
}

void SQLiteConnection::Disconnect() {

	db.reset();
}

SQLiteConnection::~SQLiteConnection(){

	Disconnect();
}
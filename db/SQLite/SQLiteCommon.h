#pragma once
#include <sqlite3.h>
#include "../DbException.h"

struct SQLiteStmtHandle{
	sqlite3 *conn;
	sqlite3_stmt *stmt;
};

class SQLiteException : public CDbException {

	int MapSQLiteErrorCodeToCommonCode(const int err_code) const;
public:
	SQLiteException(const int err_code, const Tchar *err_descr);
	SQLiteException(sqlite3 *conn);

	SQLiteException(const SQLiteException &obj) = default;
	SQLiteException(SQLiteException &&obj) = default;

	~SQLiteException();
};
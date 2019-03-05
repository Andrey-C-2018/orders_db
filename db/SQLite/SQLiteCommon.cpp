#include "SQLiteCommon.h"

SQLiteException::SQLiteException(const int err_code, const Tchar *err_descr) : \
									CDbException(err_code, err_descr) { }

int SQLiteException::MapSQLiteErrorCodeToCommonCode(const int err_code) const {

	switch (err_code) {
	case SQLITE_CONSTRAINT:
		return CDbException::E_DB_FOREIGN_KEY_DUPLICATE;

	/*case ER_PARSE_ERROR:
		return CDbException::E_DB_PARSE_ERROR;*/

	default:
		return CDbException::E_DB_WRONG_CODE;
	}
}

SQLiteException::SQLiteException(sqlite3 *conn) : \
				CDbException(MapSQLiteErrorCodeToCommonCode(sqlite3_errcode(conn)), \
									sqlite3_errmsg(conn)) { }

SQLiteException::~SQLiteException() { }
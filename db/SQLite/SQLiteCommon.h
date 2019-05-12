#pragma once
#include <sqlite3.h>
#include "../DbException.h"
#include <basic/XConv.h>

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

inline sqlite3_stmt *getTableFieldMetaInfo(sqlite3 *db, const char *table_name, \
											const size_t field, std::string &buffer) {

	buffer = "SELECT * FROM pragma_table_info('";
	buffer += table_name;
	buffer += "') WHERE cid = ";
	char conv_buffer[getDigitsCountOfType<size_t>() + 1];
	buffer += XConv::ToString(field, conv_buffer);

	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare_v2(db, buffer.c_str(), -1, &stmt, nullptr);

	return rc != SQLITE_ERROR ? stmt : nullptr;
}

inline sqlite3_stmt *getTableFieldMetaInfo(sqlite3 *db, const wchar_t *table_name, \
											const size_t field, std::wstring &buffer) {

	buffer = L"SELECT * FROM pragma_table_info('";
	buffer += table_name;
	buffer += L"') WHERE cid = ";
	wchar_t conv_buffer[getDigitsCountOfType<size_t>() + 1];
	buffer += XConv::ToString(field, conv_buffer);

	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare16_v2(db, buffer.c_str(), -1, &stmt, nullptr);

	return rc != SQLITE_ERROR ? stmt : nullptr;
}

inline int getFieldIdFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return sqlite3_column_int(stmt, 0);
}

inline const char *getFieldNameFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return (const char *)sqlite3_column_text(stmt, 1);
}

inline const wchar_t *getFieldNameWFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return (const wchar_t *)sqlite3_column_text16(stmt, 1);
}

inline const char *getFieldTypeFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return (const char *)sqlite3_column_text(stmt, 2);
}

inline bool canFieldBeNULLFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return sqlite3_column_int(stmt, 3) == 1;
}

inline bool isFieldPrimKeyFromMetaInfo(sqlite3_stmt *stmt) {

	assert(stmt);
	return sqlite3_column_int(stmt, 5) == 1;
}
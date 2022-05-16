#pragma once
#include <sqlite3.h>
#include "../DbException.h"
#include <basic/XConv.h>

struct Statement final {
	sqlite3_stmt *stmt;
	sqlite3_stmt *stmt_records_count;
	bool initial_state;
	inline Statement() noexcept : stmt(nullptr), \
									stmt_records_count(nullptr), \
									initial_state(true) { }

	inline Statement(sqlite3_stmt *stmt_, \
						sqlite3_stmt *stmt_records_count_, \
						const bool initial_state_) noexcept : \
					stmt(stmt_), \
					stmt_records_count(stmt_records_count_), \
					initial_state(initial_state_) { }

	~Statement() {

		int err = 0;
		if (stmt) {
			err = sqlite3_finalize(stmt);
			assert(err != SQLITE_ERROR);
		}

		if (stmt_records_count) {
			err = sqlite3_finalize(stmt_records_count);
			assert(err != SQLITE_ERROR);
		}
	}
};

class DBLIB_DLL_EXPORT SQLiteException : public CDbException {

	int MapSQLiteErrorCodeToCommonCode(const int err_code) const;
public:
	SQLiteException(const int err_code, const Tchar *err_descr);
	SQLiteException(sqlite3 *conn);

	SQLiteException(const SQLiteException &obj) = default;
	SQLiteException(SQLiteException &&obj) = default;

	virtual ~SQLiteException();
};

inline sqlite3_stmt *prepareQuery(sqlite3 *db, const char *query_text) {

	assert(db);

	int err = 0;
	const int QUERY_IS_NULL_TERM_STR = -1;
	sqlite3_stmt *pStmt = nullptr;

	err = sqlite3_prepare_v2(db, query_text, QUERY_IS_NULL_TERM_STR, &pStmt, NULL);
	if (err != SQLITE_OK) return nullptr;

	return pStmt;
}

inline sqlite3_stmt *getTableFieldMetaInfo(sqlite3 *db, const char *table_name, \
											const char *field_name, std::string &buffer) {

	buffer = "SELECT * FROM pragma_table_info('";
	buffer += table_name;
	buffer += "') WHERE name = '";
	buffer += field_name;
	buffer += "'";

	sqlite3_stmt *stmt = nullptr;
	int rc = sqlite3_prepare_v2(db, buffer.c_str(), -1, &stmt, nullptr);

	return rc != SQLITE_ERROR ? stmt : nullptr;
}

inline sqlite3_stmt *getTableFieldMetaInfo(sqlite3 *db, const wchar_t *table_name, \
											const wchar_t *field_name, std::wstring &buffer) {

	buffer = L"SELECT * FROM pragma_table_info('";
	buffer += table_name;
	buffer += L"') WHERE name = '";
	buffer += field_name;
	buffer += L"'";

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
	return sqlite3_column_int(stmt, 5) > 0;
}
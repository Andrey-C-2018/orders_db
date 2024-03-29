#pragma once
#include <memory>
#include <vector>
#include "../IDbResultSet.h"
#include "SQLiteCommon.h"

class DBLIB_DLL_EXPORT SQLiteResultSetException : public SQLiteException {
public:
	SQLiteResultSetException(const int err_code, const Tchar *err_descr);
	SQLiteResultSetException(sqlite3 *db);

	SQLiteResultSetException(const SQLiteResultSetException &obj);
	SQLiteResultSetException(SQLiteResultSetException &&obj) = default;

	virtual ~SQLiteResultSetException();
};

struct IDbStaticResultSet;

class SQLiteResultSet : public IDbResultSet {
	size_t fields_count, records_count;

	std::shared_ptr<sqlite3> db;
	mutable std::shared_ptr<Statement> stmt;

	mutable bool eof;
	mutable size_t fetched_record_no;

	inline int execute_stmt() const;
	inline void reloadInternal();

public:
	SQLiteResultSet(std::shared_ptr<sqlite3> db_, \
					std::shared_ptr<Statement> stmt_);

	SQLiteResultSet(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet(SQLiteResultSet &&obj) noexcept;
	SQLiteResultSet &operator=(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet &operator=(SQLiteResultSet &&obj) noexcept;

	bool empty() const override;
	size_t getFieldsCount() const override;
	size_t getRecordsCount() const override;

	void gotoRecord(const size_t record) const override;

	int getInt(const size_t field, bool &is_null) const override;
	const char *getString(const size_t field) const override;
	const wchar_t *getWString(const size_t field) const override;
	ImmutableString<char> getImmutableString(const size_t field) const override;
	ImmutableString<wchar_t> getImmutableWString(const size_t field) const override;
	CDate getDate(const size_t field, bool &is_null) const override;

	void reload() override;

	void upload(IDbStaticResultSet &static_rs) const override;

	void Release();
	virtual ~SQLiteResultSet();
};

int SQLiteResultSet::execute_stmt() const {

	stmt->initial_state = false;
	return sqlite3_step(stmt->stmt);
}

void SQLiteResultSet::reloadInternal() {

	sqlite3_reset(stmt->stmt);

	sqlite3_reset(stmt->stmt_records_count);
	int rc = sqlite3_step(stmt->stmt_records_count);
	assert(rc != SQLITE_ERROR && rc != SQLITE_DONE);

	stmt->initial_state = false;

	records_count = sqlite3_column_int(stmt->stmt_records_count, 0);
	fetched_record_no = 0;
	eof = false;
}
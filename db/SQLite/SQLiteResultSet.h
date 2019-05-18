#pragma once
#include <memory>
#include <vector>
#include "../IDbResultSet.h"
#include "SQLiteCommon.h"

class SQLiteResultSetException : public SQLiteException {
public:
	SQLiteResultSetException(const int err_code, const Tchar *err_descr);
	SQLiteResultSetException(sqlite3 *db);

	SQLiteResultSetException(const SQLiteResultSetException &obj);
	SQLiteResultSetException(SQLiteResultSetException &&obj) = default;

	~SQLiteResultSetException();
};

class SQLiteResultSet : public IDbResultSet {
	size_t fields_count, records_count;

	std::shared_ptr<sqlite3> db;
	std::shared_ptr<Statement> stmt;

	mutable bool eof;
	mutable size_t fetched_record_no;

public:
	SQLiteResultSet(std::shared_ptr<sqlite3> db_, \
					std::shared_ptr<Statement> stmt_);

	SQLiteResultSet(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet(SQLiteResultSet &&obj);
	SQLiteResultSet &operator=(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet &operator=(SQLiteResultSet &&obj);

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

	void Release();
	virtual ~SQLiteResultSet();
};


#pragma once
#include <memory>
#include <vector>
#include "../IDbResultSet.h"
#include "SQLiteCommon.h"

class SQLiteResultSet : public IDbResultSet {
	size_t fields_count, records_count;
	std::shared_ptr<SQLiteStmtHandle> stmt_handle;
	mutable size_t curr_record;

public:
	SQLiteResultSet(std::shared_ptr<SQLiteStmtHandle> stmt_handle);

	SQLiteResultSet(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet(SQLiteResultSet &&obj);
	SQLiteResultSet &operator=(const SQLiteResultSet &obj) = delete;
	SQLiteResultSet &operator=(SQLiteResultSet &&obj);

	bool empty() const override;
	size_t getFieldsCount() const override;
	size_t getRecordsCount() const override;

	void gotoRecord(const size_t record) const override;

	int getInt(const size_t field) const override;
	const char *getString(const size_t field) const override;
	const wchar_t *getWString(const size_t field) const override;
	ImmutableString<char> getImmutableString(const size_t field) const override;
	ImmutableString<wchar_t> getImmutableWString(const size_t field) const override;
	CDate getDate(const size_t field) const override;

	void Release();
	virtual ~SQLiteResultSet();
};


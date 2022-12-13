#pragma once
#include <memory>
#include <vector>
#include "../IDbResultSet.h"
#include "MySQLException.h"

class DBLIB_DLL_EXPORT CMySQLResultSetException : public CMySQLException {
public:
	enum Errors {
		E_TRUNC = 1
	};

	CMySQLResultSetException(const int err_code, const Tchar *err_descr);
	CMySQLResultSetException(MYSQL *conn);
	CMySQLResultSetException(MYSQL_STMT *stmt);

	CMySQLResultSetException(const CMySQLResultSetException &obj);
	CMySQLResultSetException(CMySQLResultSetException &&obj) = default;

	virtual ~CMySQLResultSetException();
};

struct MySQLStmtDataEx;
struct MySQLBindingItem;

class CMySQLResultSet : public IDbResultSet {
	size_t fields_count, records_count;
	std::shared_ptr<MYSQL> conn;
	mutable std::shared_ptr<const MySQLStmtDataEx> stmt;
	mutable size_t curr_record;

	std::vector<MySQLBindingItem> fields;
	std::vector<MYSQL_BIND> fields_bindings;

public:
	CMySQLResultSet(std::shared_ptr<MYSQL> conn_, \
					std::shared_ptr<const MySQLStmtDataEx> stmt_);

	CMySQLResultSet(const CMySQLResultSet &obj) = delete;
	CMySQLResultSet(CMySQLResultSet &&obj) noexcept;
	CMySQLResultSet &operator=(const CMySQLResultSet &obj) = delete;
	CMySQLResultSet &operator=(CMySQLResultSet &&obj) noexcept;

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

	virtual ~CMySQLResultSet();
};


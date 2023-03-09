#pragma once
#include "SQLiteCommon.h"
#include "../IDbStatement.h"

class DBLIB_DLL_EXPORT SQLiteStatementException : public SQLiteException {
public:
	enum {
		E_EXEC = 1
	};

	SQLiteStatementException(const int err_code, const Tchar *err_descr);
	SQLiteStatementException(sqlite3 *db);

	SQLiteStatementException(const SQLiteStatementException &obj);
	SQLiteStatementException(SQLiteStatementException &&obj) = default;

	virtual ~SQLiteStatementException();
};

class SQLiteStatement : public IDbStatement {
	std::shared_ptr<sqlite3> db;
	std::shared_ptr<Statement> stmt;
	size_t fields_count;
	size_t params_count;

	inline void resetStmtIfNecessary();
public:
	SQLiteStatement(std::shared_ptr<sqlite3> db, const char *query_text);

	SQLiteStatement(const SQLiteStatement &obj) = delete;
	SQLiteStatement(SQLiteStatement &&obj);
	SQLiteStatement &operator=(const SQLiteStatement &obj) = delete;
	SQLiteStatement &operator=(SQLiteStatement &&obj);

	size_t getParamsCount() const override { return params_count; }
	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;
	void bindNull(const size_t param_no) override;

	std::shared_ptr<IDbResultSet> exec() override;
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	virtual ~SQLiteStatement();
};

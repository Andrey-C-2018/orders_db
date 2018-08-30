#pragma once
#include <vector>
#include "IDbStatement.h"
#include "MySQLResultSet.h"

class CMySQLStatementException : public CMySQLException {
public:
	enum {
		E_EXEC = 1
	};

	CMySQLStatementException(const int err_code, const Tchar *err_descr);
	CMySQLStatementException(MYSQL *conn);
	CMySQLStatementException(MYSQL_STMT *stmt);

	CMySQLStatementException(const CMySQLStatementException &obj);
	CMySQLStatementException(CMySQLStatementException &&obj) = default;

	~CMySQLStatementException();
};

class CMySQLStatement : public IDbStatement {
	std::shared_ptr<MYSQL_STMT> stmt;
	size_t params_count;
	
	std::vector<CMySQLBindingTarget> params;
	std::vector<MYSQL_BIND> params_bindings;

	std::shared_ptr<MYSQL_RES> metadata;

	void assignParamBindingWithScalar(MYSQL_BIND &param_binding, \
										CMySQLVariant &value);
	void assignParamBindingWithVector(MYSQL_BIND &param_binding, \
										CMySQLBindingTarget &param);
public:
	CMySQLStatement(MYSQL_STMT *stmt_);
	CMySQLStatement(const CMySQLStatement &obj) = delete;
	CMySQLStatement(CMySQLStatement &&obj);
	CMySQLStatement &operator=(const CMySQLStatement &obj) = delete;
	CMySQLStatement &operator=(CMySQLStatement &&obj);

	size_t getParamsCount() const { return params_count; }
	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;

	void bindValue(const size_t param_no, const CMySQLVariant &value);
	void bindNull(const size_t param_no) override;

	std::shared_ptr<IDbResultSet> exec() override;
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	virtual ~CMySQLStatement();
};

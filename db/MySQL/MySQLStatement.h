#pragma once
#include <vector>
#include <mysql.h>
#include "../IDbStatement.h"

struct MySQLStmtDataEx;
struct MySQLBindingItem;
class CMySQLVariant;
class CMySQLResultSet;

class CMySQLStatement : public IDbStatement {

	std::shared_ptr<MYSQL> conn;
	std::shared_ptr<MySQLStmtDataEx> stmt;
	std::vector<MySQLBindingItem> params;

	void assignParamBindingWithScalar(const size_t param_no, \
										CMySQLVariant &value);
	void assignParamBindingWithVector(const size_t param_no, \
										MySQLBindingItem &param_item);
public:
	CMySQLStatement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_);

	CMySQLStatement(const CMySQLStatement &obj) = delete;
	CMySQLStatement(CMySQLStatement &&obj) noexcept;
	CMySQLStatement &operator=(const CMySQLStatement &obj) = delete;
	CMySQLStatement &operator=(CMySQLStatement &&obj) noexcept;

	void reprepare(MYSQL_STMT *stmt_);

	size_t getParamsCount() const override;
	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;

	void bindValue(const size_t param_no, const CMySQLVariant &value);
	void bindNull(const size_t param_no) override;

	std::shared_ptr<IDbResultSet> exec() override;
	std::shared_ptr<CMySQLResultSet> execMySQL();
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	virtual ~CMySQLStatement();
};

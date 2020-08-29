#pragma once
#include <vector>
#include <mysql.h>
#include "../IDbStatement.h"

struct MySQLStmtDataEx;
struct MySQLBindingItem;
class CMySQLVariant;

class CMySQLStatement : public IDbStatement {

	std::shared_ptr<MySQLStmtDataEx> stmt;
	std::vector<MySQLBindingItem> params;

	void assignParamBindingWithScalar(const size_t param_no, \
										CMySQLVariant &value);
	void assignParamBindingWithVector(const size_t param_no, \
										MySQLBindingItem &param_item);
public:
	CMySQLStatement(MYSQL_STMT *stmt_);
	CMySQLStatement(const CMySQLStatement &obj) = delete;
	CMySQLStatement(CMySQLStatement &&obj);
	CMySQLStatement &operator=(const CMySQLStatement &obj) = delete;
	CMySQLStatement &operator=(CMySQLStatement &&obj);

	size_t getParamsCount() const;
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

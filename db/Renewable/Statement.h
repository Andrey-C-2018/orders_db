#pragma once
#include "../MySQL/MySQLStatement.h"

class Statement : public IDbStatement {
	CMySQLStatement mysql_stmt;
	std::string query;

	void reconnect(CDbException &e);

public:
	Statement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_, \
				const char *query_);

	std::shared_ptr<IDbResultSet> exec() override;
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	void bindValue(const size_t param_no, const int value) override;

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, \
					const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, \
					const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;
	void bindNull(const size_t param_no) override;

	virtual ~Statement() { }
};

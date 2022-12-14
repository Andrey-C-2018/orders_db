#pragma once
#include "../MySQL/MySQLStatement.h"
#include "StmtData.h"

class Statement : public IDbStatement {
	StmtData data;

public:
	Statement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_, \
				const char *query_);

	Statement(const Statement &obj) = delete;
	Statement(Statement &&obj) noexcept;
	Statement &operator=(const Statement &obj) = delete;
	Statement &operator=(Statement &&obj) noexcept;

	std::shared_ptr<IDbResultSet> exec() override;
	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	inline size_t getParamsCount() const;
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

//*****************************************************

size_t Statement::getParamsCount() const {

	return data.getStmt()->getParamsCount();
}
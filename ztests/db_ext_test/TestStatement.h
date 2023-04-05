#pragma once
#include <db/IDbStatement.h>
#include "ITestStatement.h"
#include "TestResultSet.h"

class TestRecCountStatement;

class TestStatement : public IDbStatement, public ITestStatement {
	static constexpr size_t MAX_PARAMS_COUNT = 2;
	std::vector<int> values;
	size_t fields_count;

	std::shared_ptr<TestResultSet> rs;
	std::shared_ptr<TestRecCountStatement> stmt_rec_count;

	int bindings[MAX_PARAMS_COUNT];
	size_t bindings_counter;

public:
	TestStatement() noexcept;
	TestStatement(const int *values, size_t fields, size_t records) noexcept;

	TestStatement(const TestStatement &obj) = delete;
	TestStatement(TestStatement &&obj) = default;
	TestStatement& operator=(const TestStatement &obj) = delete;
	TestStatement& operator=(TestStatement &&obj) = default;

	std::shared_ptr<IDbStatement> getRecCountStmt();

	size_t getParamsCount() const override;
	void bindValue(const size_t param_no, const int value) override;
	int getBindedPageSize() const;
	int getBindedRecordsOffset() const;
	size_t bindIntCallsCount() const;
	void resetBindingsCounter();

	void bindValue(const size_t param_no, const char *value) override;
	void bindValue(const size_t param_no, const wchar_t *value) override;
	void bindValue(const size_t param_no, const ImmutableString<char> value) override;
	void bindValue(const size_t param_no, const ImmutableString<wchar_t> value) override;

	void bindValue(const size_t param_no, const CDate &value) override;

	void bindNull(const size_t param_no) override;

	void insertRecord(size_t pos, const int *value);
	void deleteRecord(size_t record);

	std::shared_ptr<IDbResultSet> exec() override;
	void reloadRS() override;

	record_t execScalar() override;
	std::shared_ptr<IDbResultSetMetadata> getResultSetMetadata() override;

	virtual ~TestStatement();
};
#pragma once
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "ITestStatement.h"

class TestResultSet;

class TestRecCountStatement : public IDbStatement, public ITestStatement {
	size_t records_count;
	std::shared_ptr<TestResultSet> rs_rec_count;

public:
	explicit TestRecCountStatement(size_t records_count_) noexcept;

	TestRecCountStatement(const TestRecCountStatement &obj) = delete;
	TestRecCountStatement(TestRecCountStatement &&obj) = default;
	TestRecCountStatement& operator=(const TestRecCountStatement &obj) = delete;
	TestRecCountStatement& operator=(TestRecCountStatement &&obj) = default;

	void setRecordsCount(size_t records_count_);

	size_t getParamsCount() const override;
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

	void reloadRS() override;

	virtual ~TestRecCountStatement();
};

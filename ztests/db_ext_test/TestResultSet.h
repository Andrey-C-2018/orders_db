#pragma once
#include <vector>
#include <db/IDbResultSet.h>

struct ITestStatement;

class TestResultSet : public IDbResultSet {
	ITestStatement *stmt;
	size_t fields_count;
	std::vector<std::vector<int> > values;
	mutable size_t curr_rec;

public:
	explicit TestResultSet(ITestStatement *stmt) noexcept;

	void init(const int *values, size_t fields, size_t records);

	TestResultSet(const TestResultSet &obj) = delete;
	TestResultSet(TestResultSet &&obj) = default;
	TestResultSet& operator=(const TestResultSet &obj) = delete;
	TestResultSet& operator=(TestResultSet &&obj) = delete;

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
	void upload(IDbStaticResultSet &static_rs) const override;

	virtual ~TestResultSet();
};
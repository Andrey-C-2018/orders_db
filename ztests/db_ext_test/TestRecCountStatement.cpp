#include <stdexcept>
#include "TestRecCountStatement.h"
#include "TestResultSet.h"

TestRecCountStatement::TestRecCountStatement(size_t records_count_) noexcept : \
											records_count(records_count_) { }

void TestRecCountStatement::setRecordsCount(size_t records_count_) {

	records_count = records_count_;
}

size_t TestRecCountStatement::getParamsCount() const {

	return 2;
}

void TestRecCountStatement::bindValue(const size_t param_no, const int value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindValue(const size_t param_no, const char *value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindValue(const size_t param_no, const wchar_t *value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindValue(const size_t param_no, const CDate &value) {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::bindNull(const size_t param_no) {

	throw std::logic_error("not supported");
}

std::shared_ptr<IDbResultSet> TestRecCountStatement::exec() {

	rs_rec_count = std::make_shared<TestResultSet>(this);
	reloadRS();

	return rs_rec_count;
}

record_t TestRecCountStatement::execScalar() {

	throw std::logic_error("not supported");
}

std::shared_ptr<IDbResultSetMetadata> TestRecCountStatement::getResultSetMetadata() {

	throw std::logic_error("not supported");
}

void TestRecCountStatement::reloadRS() {

	assert (rs_rec_count);
	int count = (int)records_count;
	rs_rec_count->init(&count, 1, 1);
}

TestRecCountStatement::~TestRecCountStatement() { }
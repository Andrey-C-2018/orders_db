#include <stdexcept>
#include "TestStatement.h"
#include "TestRecCountStatement.h"

TestStatement::TestStatement() noexcept : fields_count(0), bindings_counter(0) {

	bindings[0] = -1;
	bindings[1] = -1;

	rs = std::make_shared<TestResultSet>(this);
	stmt_rec_count = std::make_shared<TestRecCountStatement>(0);
}

TestStatement::TestStatement(const int *values_, size_t fields, size_t records) noexcept : \
						fields_count(fields), bindings_counter(0) {

	assert (fields || (!fields && !records && !values_));
	values.assign(values_, values_ + fields * records);

	bindings[0] = -1;
	bindings[1] = -1;

	rs = std::make_shared<TestResultSet>(this);
	stmt_rec_count = std::make_shared<TestRecCountStatement>(records);
}

std::shared_ptr<IDbStatement> TestStatement::getRecCountStmt() {

	return stmt_rec_count;
}

size_t TestStatement::getParamsCount() const {

	return 2;
}

void TestStatement::bindValue(const size_t param_no, const int value) {

	assert (param_no < MAX_PARAMS_COUNT);
	assert ((param_no == 0 && value >= 0) ||
			(param_no == 1 && ((values.empty() && value == 0) || \
								value < values.size() / fields_count)));

	bindings[param_no] = value;
	bindings_counter++;
}

size_t TestStatement::bindIntCallsCount() const {

	return bindings_counter;
}

int TestStatement::getBindedPageSize() const {

	return bindings[0];
}

int TestStatement::getBindedRecordsOffset() const {

	return bindings[1];
}

void TestStatement::resetBindingsCounter() {

	bindings_counter = 0;
}

void TestStatement::bindValue(const size_t param_no, const char *value) {

	throw std::logic_error("bindString() is not supported");
}

void TestStatement::bindValue(const size_t param_no, const wchar_t *value) {

	throw std::logic_error("bindWString() is not supported");
}

void TestStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	throw std::logic_error("bindString() is not supported");
}

void TestStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	throw std::logic_error("bindWString() is not supported");
}

void TestStatement::bindValue(const size_t param_no, const CDate &value) {

	throw std::logic_error("bindDate() is not supported");
}

void TestStatement::bindNull(const size_t param_no) {

	throw std::logic_error("bindNull() is not supported");
}

std::shared_ptr<IDbResultSet> TestStatement::exec() {

	reloadRS();
	return rs;
}

void TestStatement::insertRecord(size_t pos, const int *value) {

	assert (value);
	assert (fields_count > 0);
	assert (pos < values.size() / fields_count);

	auto p = values.begin();
	std::advance(p, pos * fields_count);
	values.insert(p, value, value + fields_count);

	stmt_rec_count->setRecordsCount(values.size() / fields_count);
	bindings[0] = -1;
	bindings[1] = -1;
}

void TestStatement::deleteRecord(size_t record) {

	assert (fields_count > 0);
	assert (record < values.size() / fields_count);

	auto p = values.begin();
	auto p_end = values.begin();
	std::advance(p, record * fields_count);
	std::advance(p_end, (record + 1) * fields_count);
	values.erase(p, p_end);

	stmt_rec_count->setRecordsCount(values.size() / fields_count);
	bindings[0] = -1;
	bindings[1] = -1;
}

void TestStatement::reloadRS() {

	if (fields_count == 0) return;
	assert (bindings[0] != -1);
	assert (bindings[1] != -1);

	size_t offset = fields_count * bindings[1];
	size_t values_size = values.size();
	size_t size = std::min((size_t)bindings[0], (values_size - offset) / fields_count);
	rs->init(&values[offset], fields_count, size);
}

record_t TestStatement::execScalar() {

	throw std::logic_error("execScalar() is not supported");
}

std::shared_ptr<IDbResultSetMetadata> TestStatement::getResultSetMetadata() {

	throw std::logic_error("execScalar() is not supported");
}

TestStatement::~TestStatement() { }

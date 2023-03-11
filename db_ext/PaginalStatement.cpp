#include <db/IDbConnection.h>
#include "PaginalStatement.h"
#include "UniqueRowIdRS.h"

PaginalStatement::PaginalStatement() : field_id_index(-1) { }

std::string& PaginalStatement::init(std::shared_ptr<IDbConnection> conn_, \
										std::string &query, int field_id_index_) {

	const char prefix[] = "SELECT COUNT(*) FROM (";
	const char postfix[] = ") a1b2c3";
	field_id_index = field_id_index_;

	auto pos = query.rfind("LIMIT");
	assert (pos == std::string::npos);

	query.insert(0, prefix);
	query += postfix;
	stmt_rec_count = conn_->PrepareQuery(query.c_str());

	query.erase(0, sizeof(prefix) - 1);
	query.erase(query.size() - sizeof(prefix) + 1, std::string::npos);

	query += " LIMIT ? OFFSET ?";
	stmt = conn_->PrepareQuery(query.c_str());

	return query;
}

size_t PaginalStatement::getParamsCount() const {

	assert (stmt_rec_count);
	return stmt_rec_count->getParamsCount();
}

void PaginalStatement::bindValue(const size_t param_no, const int value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindValue(const size_t param_no, const char *value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindValue(const size_t param_no, const wchar_t *value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindValue(const size_t param_no, const CDate &value) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindValue(param_no, value);
	stmt->bindValue(param_no, value);
}

void PaginalStatement::bindNull(const size_t param_no) {

	assert (stmt_rec_count);
	assert (stmt);
	stmt_rec_count->bindNull(param_no);
	stmt->bindNull(param_no);
}

std::shared_ptr<IDbResultSet> PaginalStatement::exec() {

	PaginalRS crs(stmt, stmt_rec_count);
	return std::make_shared<UniqueRowIdRS>(std::move(crs), field_id_index);
}

record_t PaginalStatement::execScalar() {

	return stmt->execScalar();
}

std::shared_ptr<IDbResultSetMetadata> PaginalStatement::getResultSetMetadata() {

	return stmt->getResultSetMetadata();
}

PaginalStatement::~PaginalStatement() { }
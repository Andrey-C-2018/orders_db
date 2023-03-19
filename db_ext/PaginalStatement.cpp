#include <cstring>
#include <db/IDbConnection.h>
#include "PaginalStatement.h"
#include "PaginalRS.h"

PaginalStatement::PaginalStatement(std::shared_ptr<IDbConnection> conn_, \
									const char* query, int field_id_index_) : \
							field_id_index(field_id_index_) {
	
	assert(field_id_index >= 0);
	init(conn_, query);
}

void PaginalStatement::init(std::shared_ptr<IDbConnection> conn_, \
							const char* query) {

	const char prefix[] = "SELECT COUNT(*) FROM (";
	const char postfix[] = ") a1b2c3";

	assert(query != nullptr);
	const char *p = strstr(query, "LIMIT");
	assert (p == nullptr);

	std::string query_str = prefix;
	query_str += query;
	query_str += postfix;
	stmt_rec_count = conn_->PrepareQuery(query_str.c_str());

	query_str.erase(0, sizeof(prefix) - 1);
	query_str.erase(query_str.size() - sizeof(postfix) + 1, std::string::npos);

	query_str += " LIMIT ? OFFSET ?";
	stmt = conn_->PrepareQuery(query_str.c_str());
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

	return std::make_shared<PaginalRS>(stmt, stmt_rec_count, field_id_index);
}

record_t PaginalStatement::execScalar() {

	return stmt->execScalar();
}

std::shared_ptr<IDbResultSetMetadata> PaginalStatement::getResultSetMetadata() {

	return stmt->getResultSetMetadata();
}

PaginalStatement::~PaginalStatement() { }
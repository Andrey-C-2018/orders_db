#include "PaginalStatement.h"

PaginalStatement::PaginalStatement() { }

std::string& PaginalStatement::init(std::shared_ptr<IDbConnection> conn_, \
					std::string &query) {
	const char prefix[] = "SELECT COUNT(*) FROM (";
	const char postfix[] = ") a1b2c3";

	auto pos = query.rfind("LIMIT");
	assert (pos == std::string::npos);

	query.insert(0, prefix);
	query += postfix;
	stmt_rec_count = conn_.Prepare(query.c_str());

	query.erase(0, sizeof(prefix) - 1);
	query.erase(query.size() - sizeof(prefix) + 1, std::string::npos);

	query += " LIMIT ? OFFSET ?";
	stmt = conn_.Prepare(query.c_str());
}

size_t PaginalStatement::getParamsCount() const {

	return stmt_rec_count.getParamsCount();
}

void PaginalStatement::bindValue(const size_t param_no, const int value) {

}

void PaginalStatement::bindValue(const size_t param_no, const char *value) {

}

void PaginalStatement::bindValue(const size_t param_no, const wchar_t *value) {

}

void PaginalStatement::bindValue(const size_t param_no, const ImmutableString<char> value) {

}

void PaginalStatement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

}

void PaginalStatement::bindValue(const size_t param_no, const CDate &value) {

}

void PaginalStatement::bindValue(const size_t param_no, const CMySQLVariant &value) {

}

void PaginalStatement::bindNull(const size_t param_no) {

}

std::shared_ptr<IDbResultSet> PaginalStatement::exec() {

	return std::make_shared<UniqueIdRS>(stmt, stmt_rec_count); 
}

record_t PaginalStatement::execScalar() {

}

std::shared_ptr<IDbResultSetMetadata> PaginalStatement::getResultSetMetadata() {

}

PaginalStatement::~PaginalStatement() { }
#include "../MySQL/MySQLStatementException.h"
#include "../MySQL/MySQLConnection.h"
#include "Statement.h"
#include "ResultSet.h"

Statement::Statement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_, \
						const char *query_) : \
					data(conn_, stmt_, query_) { }

Statement::Statement(Statement &&obj) noexcept : \
					data(std::move(obj.data)), \
					rs(std::move(obj.rs)) { }

Statement &Statement::operator=(Statement &&obj) noexcept {

	data = std::move(obj.data);
	rs = std::move(obj.rs);
	return *this;
}

std::shared_ptr<IDbResultSet> Statement::exec() {

	std::shared_ptr<ResultSet> ret_value = rs.lock();
	if (ret_value) {
		ret_value->reload();
		return ret_value;
	}

	try {
		ret_value = std::make_shared<ResultSet>(data, \
										data.getStmt()->execMySQL());
	}
	catch (CDbException &e) {
		data.reprepareStmt(e);
		ret_value = std::make_shared<ResultSet>(data, \
										data.getStmt()->execMySQL());
	}
	return ret_value;
}

record_t Statement::execScalar() {

	record_t recs_count;
	try {
		recs_count = data.getStmt()->execScalar();
	}
	catch (CDbException &e) {
		data.reprepareStmt(e);
		recs_count = data.getStmt()->execScalar();
	}

	return recs_count;
}

std::shared_ptr<IDbResultSetMetadata> Statement::getResultSetMetadata() {

	std::shared_ptr<IDbResultSetMetadata> meta;
	try {
		meta = data.getStmt()->getResultSetMetadata();
	}
	catch (CDbException &e) {
		data.reprepareStmt(e);
		meta = data.getStmt()->getResultSetMetadata();
	}
	return meta;
}

void Statement::bindValue(const size_t param_no, const int value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const char *value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const wchar_t *value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const CDate &value) {

	data.getStmt()->bindValue(param_no, value);
}

void Statement::bindNull(const size_t param_no) {

	data.getStmt()->bindNull(param_no);
}

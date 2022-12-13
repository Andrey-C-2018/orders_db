#include "../MySQL/MySQLStatementException.h"
#include "../MySQL/MySQLConnection.h"
#include "Statement.h"

Statement::Statement(std::shared_ptr<MYSQL> conn_, MYSQL_STMT *stmt_, \
						const char *query_) : \
					mysql_stmt(conn_, stmt_), query(query_) { }

void Statement::reconnect(CDbException &e) {

	auto conn_handle = mysql_stmt.getConnection();
	if (e.GetErrorCode() == CDbException::E_DB_CONNECTION_KILLED) {
		if (mysql_reset_connection(conn_handle.get()))
			throw CMySQLStatementException(conn_handle.get());
	}

	if (e.GetErrorCode() == CDbException::E_DB_CONNECTION_KILLED || \
		e.GetErrorCode() == CDbException::E_DB_REPREP_NEEDED) {

		MYSQL_STMT *stmt_handle = CMySQLConnection::PrepareQuery(conn_handle.get(), \
															query.c_str());
		mysql_stmt.reconnect(stmt_handle);
	}
	else throw e;
}

std::shared_ptr<IDbResultSet> Statement::exec() {

	std::shared_ptr<IDbResultSet> rs;
	try {
		rs = mysql_stmt.exec();
	}
	catch (CDbException &e) {
		reconnect(e);
		rs = mysql_stmt.exec();
	}
	return rs;
}

record_t Statement::execScalar() {

	record_t recs_count;
	try {
		recs_count = mysql_stmt.execScalar();
	}
	catch (CDbException &e) {
		reconnect(e);
		recs_count = mysql_stmt.execScalar();
	}

	return recs_count;
}

std::shared_ptr<IDbResultSetMetadata> Statement::getResultSetMetadata() {

	std::shared_ptr<IDbResultSetMetadata> meta;
	try {
		meta = mysql_stmt.getResultSetMetadata();
	}
	catch (CDbException &e) {
		reconnect(e);
		meta = mysql_stmt.getResultSetMetadata();
	}
	return meta;
}

void Statement::bindValue(const size_t param_no, const int value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const char *value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const wchar_t *value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const ImmutableString<char> value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const ImmutableString<wchar_t> value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindValue(const size_t param_no, const CDate &value) {

	mysql_stmt.bindValue(param_no, value);
}

void Statement::bindNull(const size_t param_no) {

	mysql_stmt.bindNull(param_no);
}



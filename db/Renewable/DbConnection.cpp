#include "DbConnection.h"
#include "Statement.h"

DbConnection::DbConnection() : conn_handle(nullptr) { }

void DbConnection::Connect(const char *location, const unsigned port, \
							const char *login, \
							const char *pwd, \
							const char *schema_name) {

	mysql_conn.Connect(location, port, login, pwd, schema_name);
	conn_handle = mysql_conn.getConnectionHandle();
}

void DbConnection::resetConnection(CDbException &e) const {

	if (e.GetErrorCode() == CDbException::E_DB_CONNECTION_KILLED) {
		if (mariadb_reconnect(conn_handle.get()))
			throw CMySQLConnectionException(conn_handle.get());
	}
	else throw e;
}

void DbConnection::SetSchema(const char *schema_name) {

	try {
		mysql_conn.SetSchema(schema_name);
	}
	catch (CDbException &e) {

		resetConnection(e);
		mysql_conn.SetSchema(schema_name);
	}
}

record_t DbConnection::ExecScalarQuery(const char *query_text) {

	record_t count;
	try {
		count = mysql_conn.ExecScalarQuery(query_text);
	}
	catch (CDbException &e) {

		resetConnection(e);
		count = mysql_conn.ExecScalarQuery(query_text);
	}
	return count;
}

std::shared_ptr<IDbResultSet> DbConnection::ExecQuery(const char *query_text) const {

	std::shared_ptr<IDbResultSet> rs;
	try {
		rs = ExecQueryInternal(query_text);
	}
	catch (CDbException &e) {

		resetConnection(e);
		rs = ExecQueryInternal(query_text);
	}
	return rs;
}

std::shared_ptr<IDbResultSet> DbConnection::ExecQueryInternal(const char *query_text) const {

	mysql_conn.CheckConnected();
	Statement stmt(conn_handle, \
				CMySQLConnection::PrepareQuery(conn_handle.get(), query_text), \
				query_text);
	if (stmt.getParamsCount() > 0)
		throw CMySQLConnectionException(CMySQLConnectionException::E_WRONG_QUERY, \
				_T("Parameters are forbidden in the query text passed to ExecQuery()"));

	return stmt.exec();
}

std::shared_ptr<IDbStatement> DbConnection::PrepareQuery(const char *query_text) const {

	std::shared_ptr<IDbStatement> stmt;
	try {
		mysql_conn.CheckConnected();
		MYSQL_STMT *stmt_handle = CMySQLConnection::PrepareQuery(conn_handle.get(), query_text);
		stmt = std::make_shared<Statement>(conn_handle, stmt_handle, query_text);
	}
	catch (CDbException &e) {
		resetConnection(e);
		MYSQL_STMT *stmt_handle = CMySQLConnection::PrepareQuery(conn_handle.get(), query_text);
		stmt = std::make_shared<Statement>(conn_handle, stmt_handle, query_text);
	}
	return stmt;
}

void DbConnection::Disconnect() {

	try {
		mysql_conn.Disconnect();
	}
	catch (CDbException &e) {

		resetConnection(e);
		mysql_conn.Disconnect();
	}
}

DbConnection::~DbConnection() { }

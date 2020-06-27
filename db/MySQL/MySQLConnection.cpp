#include "MySQLConnection.h"

CMySQLConnectionException::CMySQLConnectionException(const int err_code, \
											const Tchar *err_descr) : \
											CMySQLException(err_code, err_descr) { }

CMySQLConnectionException::CMySQLConnectionException(MYSQL *conn) : \
											CMySQLException(conn) { }

CMySQLConnectionException::CMySQLConnectionException(MYSQL_STMT *stmt) : \
											CMySQLException(stmt) { }

CMySQLConnectionException::CMySQLConnectionException(const CMySQLConnectionException &obj) : \
											CMySQLException(obj) { }

CMySQLConnectionException::~CMySQLConnectionException() { }

//***************************************************************

CMySQLConnection::CMySQLConnection() : conn(nullptr), connected(false) { }

void CMySQLConnection::Connect(const char *location, const unsigned port, \
								const char *login, \
								const char *pwd, \
								const char *schema_name) {
	
	Disconnect();
	conn = mysql_init(nullptr);
	if (!conn) throw CMySQLConnectionException(CMySQLConnectionException::E_INIT, \
												_T("mysql_init() : unknown error"));
	
	unsigned def_conn_timeout = DEF_CONN_TIMEOUT;
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &def_conn_timeout);
	bool no_trunc_errors_rep = true;
	mysql_options(conn, MYSQL_REPORT_DATA_TRUNCATION, &no_trunc_errors_rep);

	if (!mysql_real_connect(conn, location, login, pwd, \
							schema_name, port, NULL, 0)) {
		throw CMySQLException(conn);
	}
	connected = true;

	if (mysql_query(conn, "SET NAMES utf8"))
		throw CMySQLConnectionException(conn);
}

void CMySQLConnection::SetSchema(const char *schema_name) {

	assert(schema_name);
	std::string query = "USE ";
	query += schema_name;

	if (mysql_query(conn, query.c_str()))
		throw CMySQLConnectionException(conn);
}

record_t CMySQLConnection::ExecScalarQuery(const char *query_text) {

	CheckConnected();
	if (mysql_query(conn, query_text))
		throw CMySQLConnectionException(conn);

	return mysql_affected_rows(conn);
}

MYSQL_STMT *CMySQLConnection::InternalPrepareQuery(const char *query_text) const {

	CheckConnected();
	MYSQL_STMT *stmt(nullptr);

	stmt = mysql_stmt_init(conn);
	if (!stmt) throw CMySQLConnectionException(CMySQLConnectionException::E_OUT_OF_MEMORY, \
												_T("mysql_stmt_init(), out of memory"));

	unsigned long size = (unsigned long)strlen(query_text);
	if (mysql_stmt_prepare(stmt, query_text, size)) {
		CMySQLException e(stmt);
		
		mysql_stmt_close(stmt);
		throw e;
	}

	return stmt;
}

std::shared_ptr<IDbResultSet> CMySQLConnection::ExecQuery(const char *query_text) const {

	CMySQLStatement stmt(InternalPrepareQuery(query_text));
	if (stmt.getParamsCount() > 0)
		throw CMySQLConnectionException(CMySQLConnectionException::E_WRONG_QUERY, \
				_T("Parameters are forbidden in the query text passed to ExecQuery()"));
	return stmt.exec();
}

std::shared_ptr<IDbStatement> CMySQLConnection::PrepareQuery(const char *query_text) const {

	MYSQL_STMT *stmt = InternalPrepareQuery(query_text);
	return std::make_shared<CMySQLStatement>(stmt);
}

void CMySQLConnection::Disconnect() {

	if (conn) mysql_close(conn);
	connected = false;
	conn = nullptr;
}

CMySQLConnection::~CMySQLConnection() {

	Disconnect();
}
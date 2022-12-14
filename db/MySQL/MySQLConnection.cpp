#include "MySQLConnection.h"
#include "MySQLStatement.h"

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
	MYSQL *p_conn = mysql_init(nullptr);
	if (!p_conn) throw CMySQLConnectionException(CMySQLConnectionException::E_INIT, \
												_T("mysql_init() : unknown error"));
	conn = std::shared_ptr<MYSQL>(p_conn, [](MYSQL *p) {
		if (p) mysql_close(p);
	});
	
	unsigned def_conn_timeout = DEF_CONN_TIMEOUT;
	mysql_options(conn.get(), MYSQL_OPT_CONNECT_TIMEOUT, &def_conn_timeout);
	bool no_trunc_errors_rep = true;
	mysql_options(conn.get(), MYSQL_REPORT_DATA_TRUNCATION, &no_trunc_errors_rep);
	bool reconnect = true;
	mysql_options(conn.get(), MYSQL_OPT_RECONNECT, &reconnect);

	if (!mysql_real_connect(conn.get(), location, login, pwd, \
							schema_name, port, nullptr, 0)) {
		throw CMySQLException(conn.get());
	}
	connected = true;

	if (mysql_query(conn.get(), "SET NAMES utf8"))
		throw CMySQLConnectionException(conn.get());
}

void CMySQLConnection::SetSchema(const char *schema_name) {

	assert(schema_name);
	std::string query = "USE ";
	query += schema_name;

	if (mysql_query(conn.get(), query.c_str()))
		throw CMySQLConnectionException(conn.get());
}

record_t CMySQLConnection::ExecScalarQuery(const char *query_text) {

	CheckConnected();
	if (mysql_query(conn.get(), query_text))
		throw CMySQLConnectionException(conn.get());

	return mysql_affected_rows(conn.get());
}

MYSQL_STMT *CMySQLConnection::PrepareQuery(MYSQL *conn_handle, const char *query_text) {

	MYSQL_STMT *stmt = mysql_stmt_init(conn_handle);
	if (!stmt) throw CMySQLConnectionException(CMySQLConnectionException::E_OUT_OF_MEMORY, \
												_T("mysql_stmt_init(), out of memory"));

	auto size = (unsigned long)strlen(query_text);
	if (mysql_stmt_prepare(stmt, query_text, size)) {
		CMySQLException e(stmt);
		
		mysql_stmt_close(stmt);
		throw e;
	}

	return stmt;
}

std::shared_ptr<IDbResultSet> CMySQLConnection::ExecQuery(const char *query_text) const {

	CheckConnected();
	CMySQLStatement stmt(conn, PrepareQuery(conn.get(), query_text));
	if (stmt.getParamsCount() > 0)
		throw CMySQLConnectionException(CMySQLConnectionException::E_WRONG_QUERY, \
				_T("Parameters are forbidden in the query text passed to ExecQuery()"));
	
	return stmt.exec();
}

std::shared_ptr<IDbStatement> CMySQLConnection::PrepareQuery(const char *query_text) const {

	CheckConnected();
	MYSQL_STMT *stmt = PrepareQuery(conn.get(), query_text);
	return std::make_shared<CMySQLStatement>(conn, stmt);
}

void CMySQLConnection::Disconnect() {

	conn.reset();
	connected = false;
}

CMySQLConnection::~CMySQLConnection() {

	conn.reset();
	connected = false;
}
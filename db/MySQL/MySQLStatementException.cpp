#include "MySQLStatementException.h"

CMySQLStatementException::CMySQLStatementException(const int err_code, \
										const Tchar *err_descr) : \
										CMySQLException(err_code, err_descr) { }

CMySQLStatementException::CMySQLStatementException(MYSQL *conn) : \
										CMySQLException(conn) { }

CMySQLStatementException::CMySQLStatementException(MYSQL_STMT *stmt) : \
										CMySQLException(stmt) { }

CMySQLStatementException::CMySQLStatementException(const CMySQLStatementException &obj) : \
										CMySQLException(obj) { }

CMySQLStatementException::~CMySQLStatementException() { }
#pragma once
#include "MySQLException.h"

class CMySQLStatementException : public CMySQLException {
public:
	enum {
		E_EXEC = 1, \
		E_METADATA = 2
	};

	CMySQLStatementException(const int err_code, const Tchar *err_descr);
	CMySQLStatementException(MYSQL *conn);
	CMySQLStatementException(MYSQL_STMT *stmt);

	CMySQLStatementException(const CMySQLStatementException &obj);
	CMySQLStatementException(CMySQLStatementException &&obj) = default;

	virtual ~CMySQLStatementException();
};
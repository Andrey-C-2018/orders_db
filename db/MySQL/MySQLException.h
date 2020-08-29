#pragma once
#include "../DbException.h"
#include <mysql.h>

class CMySQLException : public CDbException {

	int MapMySQLErrorCodeToCommonCode(const int mysql_code) const;

public:
	CMySQLException(const int err_code, const Tchar *err_descr);
	CMySQLException(MYSQL *conn);
	CMySQLException(MYSQL_STMT *stmt);

	CMySQLException(const CMySQLException &obj) = default;
	CMySQLException(CMySQLException &&obj) = default;

	~CMySQLException();
};
#pragma once
#include <basic/ImmutableString.h>
#include "MySQLVariant.h"
#include "../DbException.h"

struct CMySQLBindingTarget final {
	mutable CMySQLVariant value;
	unsigned long length;
	my_bool is_null;
	my_bool error;

	CMySQLBindingTarget() : value(), length(0), \
							is_null(0), error(0) { }

	CMySQLBindingTarget(const CMySQLBindingTarget &obj) = delete;
	CMySQLBindingTarget(CMySQLBindingTarget &&obj) = default;
	CMySQLBindingTarget &operator=(const CMySQLBindingTarget &obj) = delete;
	CMySQLBindingTarget &operator=(CMySQLBindingTarget &&obj) = default;
};

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
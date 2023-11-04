#include "MySQLException.h"
#include <mysql.h>
#include <mysqld_error.h>

CMySQLException::CMySQLException(const int err_code, const Tchar *err_descr) : \
								CDbException(err_code, err_descr) { }

int CMySQLException::MapMySQLErrorCodeToCommonCode(const int mysql_code) const {

	switch (mysql_code) {
		case ER_DUP_ENTRY:
			return CDbException::E_DB_PRIMARY_KEY_DUPLICATE;

		case ER_FOREIGN_DUPLICATE_KEY_WITH_CHILD_INFO:
			return CDbException::E_DB_FOREIGN_KEY_DUPLICATE;

		case ER_PARSE_ERROR:
			return CDbException::E_DB_PARSE_ERROR;

		case ER_ROW_IS_REFERENCED_2:
			return CDbException::E_DB_PARENT_NO_ROW_UPD_DEL;

		case ER_NO_REFERENCED_ROW_2:
			return CDbException::E_DB_NO_REF_IN_MASTER_TABLE;

		case ER_CONNECTION_KILLED:
			return CDbException::E_DB_CONNECTION_KILLED;

		case 2006:
			return CDbException::E_DB_REPREP_NEEDED;

		default:
			return CDbException::E_DB_WRONG_CODE;
	}
}

CMySQLException::CMySQLException(MYSQL *conn) : \
				CDbException(MapMySQLErrorCodeToCommonCode(mysql_errno(conn)), \
								mysql_error(conn)) { }

CMySQLException::CMySQLException(MYSQL_STMT *stmt) : \
				CDbException(MapMySQLErrorCodeToCommonCode(mysql_stmt_errno(stmt)), \
								mysql_stmt_error(stmt)) { }

CMySQLException::~CMySQLException() { }
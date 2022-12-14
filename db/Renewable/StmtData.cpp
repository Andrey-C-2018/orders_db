#include <cassert>
#include <db/MySQL/MySQLConnection.h>
#include <db/MySQL/MySQLStatementException.h>
#include <db/MySQL/MySQLStatement.h>
#include "StmtData.h"

StmtData::StmtData(std::shared_ptr<MYSQL> conn_handle_, \
						MYSQL_STMT *stmt_, const char *query_) : \
					conn_handle(conn_handle_) {

	assert(query_);
	assert(conn_handle);
	assert(stmt_);
	mysql_stmt = std::make_shared<CMySQLStatement>(conn_handle_, stmt_);
	query = std::make_shared<const std::string>(query_);
}

void StmtData::reprepareStmt(CDbException &e) {

	if (e.GetErrorCode() == CDbException::E_DB_CONNECTION_KILLED) {
		if (mariadb_reconnect(conn_handle.get()))
			throw CMySQLStatementException(conn_handle.get());
	}

	if (e.GetErrorCode() == CDbException::E_DB_CONNECTION_KILLED || \
		e.GetErrorCode() == CDbException::E_DB_REPREP_NEEDED) {

		MYSQL_STMT *stmt_handle = CMySQLConnection::PrepareQuery(
							conn_handle.get(), query->c_str());
		mysql_stmt->reprepare(stmt_handle);
	}
	else throw e;
}

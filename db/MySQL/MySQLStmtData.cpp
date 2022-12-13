#include "MySQLStmtData.h"

void MySQLStmtData::restoreStmt(MYSQL_STMT *stmt_) {

	assert(stmt);
	assert(stmt_);
	release();

	stmt = stmt_;
	metadata = mysql_stmt_result_metadata(stmt);
	if (!metadata) {
		if (mysql_stmt_errno(stmt))
			throw CMySQLStatementException(stmt);
	}
}

MySQLStmtData::~MySQLStmtData() {

	release();
}
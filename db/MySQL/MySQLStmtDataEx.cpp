#include "MySQLStmtDataEx.h"

void MySQLStmtDataEx::restoreStmt(MYSQL_STMT *stmt_) {

	MySQLStmtData::restoreStmt(stmt_);
	assert(params_count == mysql_stmt_param_count(stmt));
}

MySQLStmtDataEx::~MySQLStmtDataEx() {

	if (params_bindings) {
		assert(params_count > 0);
		delete[] params_bindings;
	}
}
#include "MySQLStmtData.h"

MySQLStmtData::~MySQLStmtData() {

	if (metadata) mysql_free_result(metadata);
	if (stmt) mysql_stmt_close(stmt);
}
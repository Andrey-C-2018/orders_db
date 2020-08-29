#pragma once
#include "MySQLStatementException.h"

struct MySQLStmtData {
	MYSQL_STMT *stmt;
	MYSQL_RES *metadata;

	inline MySQLStmtData() noexcept;
	inline MySQLStmtData(MYSQL_STMT *stmt_);

	inline MySQLStmtData(const MySQLStmtData &obj) = delete;
	inline MySQLStmtData(MySQLStmtData &&obj) noexcept;
	inline MySQLStmtData &operator=(const MySQLStmtData &obj) = delete;
	inline MySQLStmtData &operator=(MySQLStmtData &&obj) noexcept;
	
	virtual ~MySQLStmtData();
};

//*****************************************************

MySQLStmtData::MySQLStmtData() noexcept : stmt(nullptr), metadata(nullptr) { }

MySQLStmtData::MySQLStmtData(MYSQL_STMT *stmt_) : stmt(stmt_) {

	assert(stmt);
	metadata = mysql_stmt_result_metadata(stmt);
	if (!metadata) {
		if (mysql_stmt_errno(stmt))
			throw CMySQLStatementException(stmt);
	}
}

MySQLStmtData::MySQLStmtData(MySQLStmtData &&obj) noexcept : stmt(obj.stmt), \
														metadata(obj.metadata) {

	obj.stmt = nullptr;
	obj.metadata = nullptr;
}

inline MySQLStmtData &MySQLStmtData::operator=(MySQLStmtData &&obj) noexcept {

	stmt = obj.stmt;
	metadata = obj.metadata;

	obj.stmt = nullptr;
	obj.metadata = nullptr;
}
#pragma once
#include "MySQLStatementException.h"

struct MySQLStmtData {
	MYSQL_STMT *stmt;
	MYSQL_RES *metadata;

	inline MySQLStmtData() noexcept;
	inline explicit MySQLStmtData(MYSQL_STMT *stmt_);

	inline MySQLStmtData(const MySQLStmtData &obj) = delete;
	inline MySQLStmtData(MySQLStmtData &&obj) noexcept;
	inline MySQLStmtData &operator=(const MySQLStmtData &obj) = delete;
	inline MySQLStmtData &operator=(MySQLStmtData &&obj) noexcept;

	virtual void restoreStmt(MYSQL_STMT *stmt_);
	inline void release();
	
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
	return *this;
}

void MySQLStmtData::release() {

	if (metadata) {
		mysql_free_result(metadata);
		metadata = nullptr;
	}
	if (stmt) {
		mysql_stmt_close(stmt);
		stmt = nullptr;
	}
}
#pragma once
#include <memory>
#include <mysql.h>

class CMySQLStatement;
class CDbException;
class CMySQLResultSet;

class StmtData final {
	std::shared_ptr<MYSQL> conn_handle;
	std::shared_ptr<CMySQLStatement> mysql_stmt;
	std::shared_ptr<const std::string> query;

public:
	StmtData(std::shared_ptr<MYSQL> conn_handle_, \
				MYSQL_STMT *stmt_, \
				const char *query_);

	StmtData(const StmtData &obj) = default;
	StmtData(StmtData &&obj) noexcept = default;
	StmtData &operator=(const StmtData &obj) = default;
	StmtData &operator=(StmtData &&obj) noexcept = default;

	inline CMySQLStatement *getStmt();
	inline const CMySQLStatement *getStmt() const;

	void reprepareStmt(CDbException &e);
};

//*****************************************************

CMySQLStatement *StmtData::getStmt() {

	return mysql_stmt.get();
}

const CMySQLStatement *StmtData::getStmt() const {

	return mysql_stmt.get();
}

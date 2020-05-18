#pragma once
#include <memory>
#include <map>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "MetaInfo.h"

class CQuery{
	CMetaInfo meta_info;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt;
	
	mutable std::map<size_t, std::shared_ptr<IDbStatement> > update_stmts;
public:
	CQuery(std::shared_ptr<IDbConnection> conn_, \
			std::shared_ptr<IDbStatement> stmt_);

	CQuery(const CQuery &obj) = delete;
	CQuery(CQuery &&obj) = default;
	CQuery &operator=(const CQuery &obj) = delete;
	CQuery &operator=(CQuery &&obj) = default;
	
	inline const CMetaInfo &getMetaInfo() const noexcept;
	inline void setPrimaryTable(const char *table_name);
	inline void markFieldAsPrimaryKey(const size_t field);

	std::shared_ptr<IDbStatement> \
		getUpdateStmt(const size_t updated_field, \
				std::shared_ptr<IDbResultSet> result_set) const;

	std::shared_ptr<IDbStatement> \
		getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const;

	inline std::shared_ptr<IDbBindingTarget> getBindingTarget();
	inline std::shared_ptr<IDbResultSet> exec();
	void setStmtWithTheSameMetaInfo(std::shared_ptr<IDbStatement> stmt);

	~CQuery();
};

//*****************************************************

const CMetaInfo &CQuery::getMetaInfo() const noexcept {

	return meta_info;
}

void CQuery::setPrimaryTable(const char *table_name) {

	meta_info.setPrimaryTable(table_name);
}

void CQuery::markFieldAsPrimaryKey(const size_t field) {

	meta_info.markFieldAsPrimaryKey(field);
}

std::shared_ptr<IDbBindingTarget> CQuery::getBindingTarget() {

	return stmt;
}

std::shared_ptr<IDbResultSet> CQuery::exec() {

	return stmt->exec();
}
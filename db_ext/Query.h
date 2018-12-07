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

	std::shared_ptr<IDbStatement> \
		getUpdateStmt(const size_t updated_field, \
				std::shared_ptr<IDbResultSet> result_set) const;

	std::shared_ptr<IDbStatement> \
		getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const;

	std::shared_ptr<IDbBindingTarget> getBindingTarget();
	std::shared_ptr<IDbResultSet> exec();

	~CQuery();
};

//*****************************************************

const CMetaInfo &CQuery::getMetaInfo() const noexcept {

	return meta_info;
}
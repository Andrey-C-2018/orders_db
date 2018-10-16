#pragma once
#include <memory>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "MetaInfo.h"

class CQuery{
	CMetaInfo meta_info;
	std::shared_ptr<IDbStatement> stmt;
	
	struct CUpdateStatementPool {
		size_t field;
		std::shared_ptr<IDbStatement> stmt;
	};
public:
	CQuery(std::shared_ptr<IDbStatement> stmt_);
	
	inline const CMetaInfo &getMetaInfo() const;

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

const CMetaInfo &CQuery::getMetaInfo() const {

	return meta_info;
}
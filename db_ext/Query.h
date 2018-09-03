#pragma once
#include <memory>
#include <db/IDbStatement.h>

class CQuery{
	CMetaInfo meta_info;
	std::shared_ptr<IDbStatement> stmt;
	
	class CDbStatementPool;
public:
	CQuery(std::shared_ptr<IDbStatement> stmt_);
	
	const CMetaInfo &getMetaInfo() const;

	std::shared_ptr<IDbStatement> \
		getUpdateStmt(const size_t updated_field, \
				std::shared_ptr<IDbStatement> result_set) const;

	std::shared_ptr<IDbStatement> \
		getDeleteStmt(std::shared_ptr<IDbStatement> result_set) const;

	std::shared_ptr<IDbBindingTarget> getBindingTarget();
	std::shared_ptr<IDbResultSet> exec();
};

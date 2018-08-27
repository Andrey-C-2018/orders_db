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

	template <class BindValuePredicate> std::shared_ptr<IDbStatement> \
		getCurrRecordUpdateStmt(const size_t updated_field, \
					const BindValuePredicate binding_pred) const;

	template <class BindValuePredicate> std::shared_ptr<IDbStatement> \
		getCurrRecordDeleteStmt(const BindValuePredicate binding_pred) const;

	std::shared_ptr<IDbBindingTarget> getBindingTarget();
	std::shared_ptr<IDbResultSet> exec();
};

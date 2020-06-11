#pragma once
#include <memory>
#include <map>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "MetaInfo.h"

class CRevDbResultSet;

class CQuery{
	CMetaInfo meta_info;
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt;
	std::shared_ptr<CRevDbResultSet> rs;
	
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
	inline void addPrimaryKeyAsRef(const char *field_name, const char *table_name, \
									const char *referenved_field_name, \
									const char *referenced_table_name);

	std::shared_ptr<IDbStatement> \
		getUpdateStmt(const size_t updated_field, \
				std::shared_ptr<IDbResultSet> result_set) const;

	std::shared_ptr<IDbStatement> \
		getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const;

	inline std::shared_ptr<IDbBindingTarget> getBindingTarget();
	std::shared_ptr<IDbResultSet> exec(const bool rev_sorting_order);
	void setStmtWithTheSameMetaInfo(std::shared_ptr<IDbStatement> stmt);

	void setSortingOrder(const bool rev_sorting_order);

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

void CQuery::addPrimaryKeyAsRef(const char *field_name, const char *table_name, \
								const char *referenved_field_name, \
								const char *referenced_table_name) {

	meta_info.addInvisibleFieldAsRef(field_name, table_name, \
									referenved_field_name, 
									referenced_table_name, true);
}

std::shared_ptr<IDbBindingTarget> CQuery::getBindingTarget() {

	return stmt;
}
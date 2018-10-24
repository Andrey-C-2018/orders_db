#include "Query.h"

CQuery::CQuery(std::shared_ptr<IDbConnection> conn_, \
				std::shared_ptr<IDbStatement> stmt_) : conn(conn_), stmt(stmt_) {

	auto fields = stmt->getResultSetMetadata();
	meta_info.clearAndAddFields(fields);
}

std::shared_ptr<IDbStatement> CQuery::getUpdateStmt(const size_t updated_field, \
									std::shared_ptr<IDbResultSet> result_set) const {
	auto p = update_stmts.lower_bound(updated_field);

	std::shared_ptr<IDbStatement> update_stmt;
	if (p == update_stmts.end() || (update_stmts.key_comp()(updated_field, p->first))) {
		std::string query;

		meta_info.getUpdateQueryForField(updated_field, query);
		update_stmt = conn->PrepareQuery(query.c_str());
		update_stmts[updated_field] = update_stmt;
	}

	meta_info.bindPrimaryKeyValues(updated_field, result_set, update_stmt);
	return update_stmt;
}

std::shared_ptr<IDbStatement> CQuery::getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const {
	std::shared_ptr<IDbStatement> delete_stmt;
	std::string query;

	meta_info.getDeleteQuery( query );
	delete_stmt = conn->PrepareQuery(query.c_str());

	meta_info.bindPrimaryKeyValues(result_set, delete_stmt);
	return delete_stmt;
}

std::shared_ptr<IDbBindingTarget> CQuery::getBindingTarget() {

	return stmt;
}

std::shared_ptr<IDbResultSet> CQuery::exec() {

	return stmt->exec();
}

CQuery::~CQuery(){ }
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "MetaInfo.h"
#include "ScalarQueryCache.h"

ScalarQueryCache::ScalarQueryCache(std::shared_ptr<IDbConnection> conn_, \
									const CMetaInfo &meta_info_) : \
								conn(conn_), \
								meta_info(meta_info_) { }

std::shared_ptr<IDbStatement> ScalarQueryCache::getUpdateStmt(const size_t updated_field, \
									std::shared_ptr<IDbResultSet> result_set) const {
	auto p = update_stmts_cache.lower_bound(updated_field);

	std::shared_ptr<IDbStatement> update_stmt;
	if (p == update_stmts_cache.end() || (update_stmts_cache.key_comp()(updated_field, p->first))) {
		std::string query;

		meta_info.getUpdateQueryForField(updated_field, query);
		update_stmt = conn->PrepareQuery(query.c_str());
		update_stmts_cache[updated_field] = update_stmt;
	}
	else
		update_stmt = p->second;

	meta_info.bindPrimaryKeyValuesWithOffset(updated_field, 1, result_set, update_stmt);
	return update_stmt;
}

std::shared_ptr<IDbStatement> ScalarQueryCache::getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const {
	std::shared_ptr<IDbStatement> delete_stmt;
	std::string query;

	meta_info.getDeleteQuery( query );
	delete_stmt = conn->PrepareQuery(query.c_str());

	meta_info.bindPrimaryKeyValues(result_set, delete_stmt);
	return delete_stmt;
}

ScalarQueryCache::~ScalarQueryCache() { }
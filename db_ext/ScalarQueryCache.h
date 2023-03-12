#pragma once
#include <memory>
#include <map>

struct IDbConnection;
struct IDbStatement;
struct IDbResultSet;
class CMetaInfo;

class ScalarQueryCache final {
	const CMetaInfo &meta_info;
	std::shared_ptr<IDbConnection> conn;

	mutable std::map<size_t, std::shared_ptr<IDbStatement> > update_stmts_cache;

public:
	ScalarQueryCache(std::shared_ptr<IDbConnection> conn_, \
						const CMetaInfo &meta_info_);

	ScalarQueryCache(const ScalarQueryCache &obj) = delete;
	ScalarQueryCache(ScalarQueryCache &&obj) = default;
	ScalarQueryCache &operator=(const ScalarQueryCache &obj) = delete;
	ScalarQueryCache &operator=(ScalarQueryCache &&obj) = delete;
	
	std::shared_ptr<IDbStatement> \
		getUpdateStmt(size_t updated_field, \
				std::shared_ptr<IDbResultSet> result_set) const;

	std::shared_ptr<IDbStatement> \
		getDeleteStmt(std::shared_ptr<IDbResultSet> result_set) const;

	~ScalarQueryCache();
};

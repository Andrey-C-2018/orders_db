#pragma once
#include <memory>
#include "../IDbResultSetMetadata.h"
#include "SQLiteCommon.h"

class SQLiteResultSetMetadata : public IDbResultSetMetadata {
	std::shared_ptr<SQLiteStmtHandle> stmt_handle;
	size_t fields_count;

public:
	SQLiteResultSetMetadata(std::shared_ptr<SQLiteStmtHandle> stmt_handle_);

	SQLiteResultSetMetadata(const SQLiteResultSetMetadata &obj) = delete;
	SQLiteResultSetMetadata(SQLiteResultSetMetadata &&obj) = default;
	SQLiteResultSetMetadata &operator=(const SQLiteResultSetMetadata &obj) = delete;
	SQLiteResultSetMetadata &operator=(SQLiteResultSetMetadata &&obj) = default;

	size_t getFieldsCount() const override;
	std::shared_ptr<IDbField> getField(const size_t field) const override;

	virtual ~SQLiteResultSetMetadata();
};
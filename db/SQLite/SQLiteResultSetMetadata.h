#pragma once
#include <memory>
#include "../IDbResultSetMetadata.h"
#include "SQLiteCommon.h"

struct SQLiteFieldMetadata {
	int id;
	std::string name;
	bool is_null;
	bool is_primary_key;
};

class SQLiteResultSetMetadata : public IDbResultSetMetadata {
	std::shared_ptr<sqlite3> db;
	std::shared_ptr<Statement> stmt;
	size_t fields_count;

public:
	SQLiteResultSetMetadata(std::shared_ptr<sqlite3> db_, \
							std::shared_ptr<Statement> stmt_);

	SQLiteResultSetMetadata(const SQLiteResultSetMetadata &obj) = delete;
	SQLiteResultSetMetadata(SQLiteResultSetMetadata &&obj) = default;
	SQLiteResultSetMetadata &operator=(const SQLiteResultSetMetadata &obj) = delete;
	SQLiteResultSetMetadata &operator=(SQLiteResultSetMetadata &&obj) = default;

	size_t getFieldsCount() const override;
	std::shared_ptr<IDbField> getField(const size_t field) const override;

	virtual ~SQLiteResultSetMetadata();
};
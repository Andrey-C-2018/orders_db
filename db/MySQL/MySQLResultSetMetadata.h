#pragma once
#include <memory>
#include <basic/XString.h>
#include "../IDbResultSetMetadata.h"
#include "MySQLCommon.h"

class CMySQLResultSetMetadata :	public IDbResultSetMetadata{
	std::shared_ptr<MYSQL_RES> metadata;
	size_t fields_count;

public:
	CMySQLResultSetMetadata(std::shared_ptr<MYSQL_RES> metadata_);

	CMySQLResultSetMetadata(const CMySQLResultSetMetadata &obj) = delete;
	CMySQLResultSetMetadata(CMySQLResultSetMetadata &&obj) = default;
	CMySQLResultSetMetadata &operator=(const CMySQLResultSetMetadata &obj) = delete;
	CMySQLResultSetMetadata &operator=(CMySQLResultSetMetadata &&obj) = default;

	size_t getFieldsCount() const override;
	std::shared_ptr<IDbField> getField(const size_t field) const override;

	virtual ~CMySQLResultSetMetadata();
};

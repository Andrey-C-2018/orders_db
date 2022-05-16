#pragma once
#include <memory>
#include "../IDbResultSetMetadata.h"
#include "MySQLException.h"

class DBLIB_DLL_EXPORT CMySQLMetadataException : public CMySQLException {
public:
	enum Errors {
		E_STMT_RELEASED = 1
	};

	CMySQLMetadataException(const int err_code, const Tchar *err_descr);
	CMySQLMetadataException(MYSQL *conn);
	CMySQLMetadataException(MYSQL_STMT *stmt);

	CMySQLMetadataException(const CMySQLMetadataException &obj);
	CMySQLMetadataException(CMySQLMetadataException &&obj) = default;

	virtual ~CMySQLMetadataException();
};

struct MySQLStmtData;

class CMySQLResultSetMetadata :	public IDbResultSetMetadata{
	std::weak_ptr<const MySQLStmtData> stmt;
	size_t fields_count;

public:
	CMySQLResultSetMetadata(std::shared_ptr<const MySQLStmtData> stmt_);

	CMySQLResultSetMetadata(const CMySQLResultSetMetadata &obj) = delete;
	CMySQLResultSetMetadata(CMySQLResultSetMetadata &&obj) = default;
	CMySQLResultSetMetadata &operator=(const CMySQLResultSetMetadata &obj) = delete;
	CMySQLResultSetMetadata &operator=(CMySQLResultSetMetadata &&obj) = default;

	size_t getFieldsCount() const override;
	std::shared_ptr<IDbField> getField(const size_t field) const override;

	virtual ~CMySQLResultSetMetadata();
};

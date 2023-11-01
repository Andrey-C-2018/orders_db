#pragma once
#include <memory>
#include "IDbCommon.h"

struct IDbResultSet;
struct IDbStatement;

struct IDbConnection {
	virtual void Connect(const char *location, const unsigned port, \
							const char *login, \
							const char *pwd, \
							const char *schema_name) = 0;
	virtual void SetSchema(const char *schema_name) = 0;

	virtual record_t ExecScalarQuery(const char *query_text) = 0;
	virtual std::shared_ptr<IDbResultSet> ExecQuery(const char *query_text) const = 0;
	virtual std::shared_ptr<IDbStatement> PrepareQuery(const char *query_text) const = 0;
    virtual unsigned getLastInsertedId() const = 0;

    virtual void setAutocommitMode(bool enabled) = 0;
    virtual void commit() = 0;
    virtual void rollback() = 0;

	virtual void Disconnect() = 0;
	virtual ~IDbConnection() { }
};

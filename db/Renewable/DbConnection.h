#pragma once
#include <db/MySQL/MySQLConnection.h>

class DbConnection : public IDbConnection {
	CMySQLConnection mysql_conn;
	mutable std::shared_ptr<MYSQL> conn_handle;

	void resetConnection(CDbException &e) const;
	std::shared_ptr<IDbResultSet> ExecQueryInternal(const char *query_text) const;

public:
	DbConnection();

	DbConnection(const DbConnection &obj) = delete;
	DbConnection(DbConnection &&obj) = default;
	DbConnection &operator=(const DbConnection &obj) = delete;
	DbConnection &operator=(DbConnection &&obj) = default;

	void Connect(const char *location, const unsigned port, \
							const char *login, \
							const char *pwd, \
							const char *schema_name) override;
	void SetSchema(const char *schema_name) override;

	record_t ExecScalarQuery(const char *query_text) override;
	std::shared_ptr<IDbResultSet> ExecQuery(const char *query_text) const override;
	std::shared_ptr<IDbStatement> PrepareQuery(const char *query_text) const override;

	void Disconnect() override;
	virtual ~DbConnection();
};
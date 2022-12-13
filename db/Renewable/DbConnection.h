#pragma once
#include "../MySQL/MySQLConnection.h"

class DbConnection : public IDbConnection {
	CMySQLConnection mysql_conn;
	mutable std::shared_ptr<MYSQL> conn_handle;

	void resetConnection(CDbException &e) const;

public:
	DbConnection();

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

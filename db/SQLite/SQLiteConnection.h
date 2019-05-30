#pragma once
#include "../IDbConnection.h"
#include "SQLiteCommon.h"

class SQLiteConnectionException : public SQLiteException {
public:
	enum {
		E_WRONG_QUERY = 1, \
		E_NOT_A_SCALAR_QUERY = 2, \
		E_CONNECTION_PARAMS = 3
	};

	SQLiteConnectionException(const int err_code, const Tchar *err_descr);
	SQLiteConnectionException(sqlite3 *db);

	SQLiteConnectionException(const SQLiteConnectionException &obj);
	SQLiteConnectionException(SQLiteConnectionException &&obj) = default;

	~SQLiteConnectionException();
};

class SQLiteConnection : public IDbConnection {
	std::shared_ptr<sqlite3> db;

public:
	SQLiteConnection();
	SQLiteConnection(const SQLiteConnection &obj) = delete;
	SQLiteConnection(SQLiteConnection &&obj) = default;

	SQLiteConnection &operator=(const SQLiteConnection &obj) = delete;
	SQLiteConnection &operator=(SQLiteConnection &&obj) = default;

	void Connect(const char *location, const unsigned port, \
					const char *login, \
					const char *pwd, \
					const char *schema_name) override;
	void SetSchema(const char *schema_name) override;

	record_t ExecScalarQuery(const char *query_text) override;
	std::shared_ptr<IDbResultSet> ExecQuery(const char *query_text) const override;
	std::shared_ptr<IDbStatement> PrepareQuery(const char *query_text) const override;

	void Disconnect() override;

	virtual ~SQLiteConnection();
};


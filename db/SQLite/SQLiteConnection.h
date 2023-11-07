#pragma once
#include "../IDbConnection.h"
#include "SQLiteCommon.h"

class DBLIB_DLL_EXPORT SQLiteConnectionException : public SQLiteException {
public:
	enum {
		E_WRONG_QUERY = 1, \
		E_NOT_A_SCALAR_QUERY = 2, \
		E_CONNECTION_PARAMS = 3
	};

	SQLiteConnectionException(const int err_code, const Tchar *err_descr);
	explicit SQLiteConnectionException(sqlite3 *db);

	SQLiteConnectionException(const SQLiteConnectionException &obj);
	SQLiteConnectionException(SQLiteConnectionException &&obj) = default;

	virtual ~SQLiteConnectionException();
};

class DBLIB_DLL_EXPORT SQLiteConnection : public IDbConnection {
	std::shared_ptr<sqlite3> db;
    void executeSQLCommand(const char *cmd);

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

    record_t getLastInsertedId() const override;

    void setAutocommitMode(bool enabled) override;
    void commit() override;
    void rollback() override;

	void Disconnect() override;

	virtual ~SQLiteConnection();
};


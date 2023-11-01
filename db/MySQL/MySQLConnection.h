#pragma once
#include "../IDbConnection.h"
#include "MySQLException.h"

class DBLIB_DLL_EXPORT CMySQLConnectionException : public CMySQLException {
public:
	enum {
		E_INIT = 1, \
		E_OUT_OF_MEMORY = 2, \
		E_WRONG_QUERY = 3, \
		E_CONNECTED = 4, \
        E_TRANSACTION = 5
	};

	CMySQLConnectionException(const int err_code, const Tchar *err_descr);
	explicit CMySQLConnectionException(MYSQL *conn);
    explicit CMySQLConnectionException(MYSQL_STMT *stmt);

	CMySQLConnectionException(const CMySQLConnectionException &obj);
	CMySQLConnectionException(CMySQLConnectionException &&obj) = default;

	virtual ~CMySQLConnectionException();
};

class DBLIB_DLL_EXPORT CMySQLConnection : public IDbConnection {
	enum {
		DEF_CONN_TIMEOUT = 5
	};
	std::shared_ptr<MYSQL> conn;
	bool connected;

public:
	CMySQLConnection();
	CMySQLConnection(const CMySQLConnection &obj) = delete;
	CMySQLConnection(CMySQLConnection &&obj) = default;

	CMySQLConnection &operator=(const CMySQLConnection &obj) = delete;
	CMySQLConnection &operator=(CMySQLConnection &&obj) = default;

	inline void CheckConnected() const;
	void Connect(const char *location, const unsigned port, \
					const char *login, \
					const char *pwd, \
					const char *schema_name) override;
	void SetSchema(const char *schema_name) override;

	record_t ExecScalarQuery(const char *query_text) override;
	std::shared_ptr<IDbResultSet> ExecQuery(const char *query_text) const override;
	std::shared_ptr<IDbStatement> PrepareQuery(const char *query_text) const override;
	static MYSQL_STMT *PrepareQuery(MYSQL *conn_handle, const char *query_text);

    unsigned getLastInsertedId() const override;

    void setAutocommitMode(bool enabled) override;
    void commit() override;
    void rollback() override;

	void Disconnect() override;
	inline std::shared_ptr<MYSQL> getConnectionHandle();

	virtual ~CMySQLConnection();
};

//*****************************************************

void CMySQLConnection::CheckConnected() const {

	if (!connected)	
		throw CMySQLConnectionException(CMySQLConnectionException::E_CONNECTED, \
										_T("No connection to a MySQL database"));
}

std::shared_ptr<MYSQL> CMySQLConnection::getConnectionHandle() {

	return conn;
}

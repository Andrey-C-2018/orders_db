#pragma once
#include <memory>

struct IDbConnection;

class CDbConnectionFactory {
	static CDbConnectionFactory instance;

	CDbConnectionFactory();
	CDbConnectionFactory(const CDbConnectionFactory &obj) = delete;
	CDbConnectionFactory &operator=(const CDbConnectionFactory &obj) = delete;
public:
	enum ConnectionTypes {
		DB_CONN_MYSQL = 1,
		DB_CONN_SQLITE = 2
	};

	static inline const CDbConnectionFactory &getInstance() { return instance; }
	std::shared_ptr<IDbConnection> createConnection(const int conn_type) const;

	virtual ~CDbConnectionFactory();
};


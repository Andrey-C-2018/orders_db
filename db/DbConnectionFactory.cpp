#include "DbConnectionFactory.h"
#include "MySQL/MySQLConnection.h"

CDbConnectionFactory CDbConnectionFactory::instance;

CDbConnectionFactory::CDbConnectionFactory(){ }

std::shared_ptr<IDbConnection> \
	CDbConnectionFactory::createConnection(const int conn_type) const {

	switch (conn_type) {
		case DB_CONN_MYSQL: 
			return std::make_shared<CMySQLConnection>();

		default:
			XException e(0, _T("This connection type is not supported: "));
			e << conn_type;
			throw e;
	}
}

CDbConnectionFactory::~CDbConnectionFactory(){ }

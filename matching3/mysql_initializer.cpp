#include "mysql_initializer.h"
#include <basic/PropertiesFile.h>
#include <db/MySQL/MySQLConnection.h>

std::shared_ptr<IDbConnection> createMySQLConnection(const CPropertiesFile &props) {
	
	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	Tstring buffer;
	const Tchar *curr_prop = nullptr;
	std::string server, user, pwd, database;

	curr_prop = props.getStringProperty(_T("address"), buffer);
	const char *p_server = UCS16_ToUTF8(curr_prop, -1, server);

	int port = props.getIntProperty(_T("port"), buffer);

	curr_prop = props.getStringProperty(_T("user"), buffer);
	const char *p_user = UCS16_ToUTF8(curr_prop, -1, user);

	curr_prop = props.getStringProperty(_T("password"), buffer);
	const char *p_pwd = UCS16_ToUTF8(curr_prop, -1, pwd);

	curr_prop = props.getStringProperty(_T("database"), buffer);
	const char *p_database = UCS16_ToUTF8(curr_prop, -1, database);

	conn->Connect(p_server, port, p_user, p_pwd, p_database);
	return conn;
}

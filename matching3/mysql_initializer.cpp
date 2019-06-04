#include "mysql_initializer.h"
#include <basic/PropertiesFile.h>
#include <db/MySQL/MySQLConnection.h>

enum {
	MAX_SIZE = 200
};

void getPropertyValue(const CPropertiesFile &props, \
						const Tchar *prop_name, const Tchar *prop_descr, \
						Tstring &buffer, std::string &dest) {

	const Tchar *curr_prop = props.getStringProperty(prop_name, buffer);
	if (!curr_prop || (curr_prop && curr_prop[0] == _T('\0'))) {
		XException e(0, _T("No '"));
		e << prop_name<< _T("' property for ") << prop_descr << _T(" in config.ini");
		throw e;
	}

	UCS16_ToUTF8(curr_prop, -1, dest);
}

std::shared_ptr<IDbConnection> createMySQLConnection(const CPropertiesFile &props) {
	
	std::shared_ptr<IDbConnection> conn = std::make_shared<CMySQLConnection>();

	Tstring buffer;
	std::string server, user, pwd, database;

	getPropertyValue(props, _T("address"), _T("MySQL server address"), buffer, server);
	int port = props.getIntProperty(_T("port"), buffer);
	getPropertyValue(props, _T("mysql_user"), _T("MySQL server user"), buffer, user);
	getPropertyValue(props, _T("mysql_password"), _T("MySQL server password"), buffer, pwd);
	getPropertyValue(props, _T("database"), _T("MySQL server database"), buffer, database);

	conn->Connect(server.c_str(), port, user.c_str(), pwd.c_str(), database.c_str());
	return conn;
}

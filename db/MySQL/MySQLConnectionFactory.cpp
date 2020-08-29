#include "MySQLConnectionFactory.h"
#include <basic/PropertiesFile.h>
#include <basic/Cryptor.h>
#include <basic/TextConv.h>

inline const Tchar *getPropertyValue(const CPropertiesFile &props, \
						const Tchar *prop_name, const Tchar *prop_descr, \
						Tstring &buffer) {

	const Tchar *curr_prop = props.getStringProperty(prop_name, buffer);
	if (!curr_prop) {
		XException e(0, _T("No '"));
		e << prop_name << _T("' property for ") << prop_descr << _T(" in config.ini");
		throw e;
	}
	if (curr_prop && curr_prop[0] == _T('\0')) {
		XException e(0, _T("The property '"));
		e << prop_name << _T("' used to get ") << prop_descr << _T(" is empty in config.ini");
		throw e;
	}

	return curr_prop;
}

inline void getPropertyValue(const CPropertiesFile &props, \
						const Tchar *prop_name, const Tchar *prop_descr, \
						Tstring &buffer, std::string &dest) {

	UCS16_ToUTF8(getPropertyValue(props, prop_name, prop_descr, buffer), -1, dest);
}

//*****************************************************

CMySQLConnectionFactory::CMySQLConnectionFactory() { }

CMySQLConnectionFactory::Properties \
CMySQLConnectionFactory::getProperties(const CPropertiesFile &props, \
										Tstring &buffer) {

	CMySQLConnectionFactory::Properties p;

	getPropertyValue(props, _T("address"), _T("MySQL server address"), buffer, p.server);
	p.port = props.getIntProperty(_T("port"), buffer);
	getPropertyValue(props, _T("mysql_user"), _T("MySQL server user"), buffer, p.user);
	getPropertyValue(props, _T("database"), _T("database name"), buffer, p.database);

	return p;
}

CMySQLConnectionPtr CMySQLConnectionFactory::createConnection(const CPropertiesFile &props) {

	Tstring buffer;
	CMySQLConnectionFactory::Properties p = getProperties(props, buffer);
	getPropertyValue(props, _T("mysql_password"), \
						_T("MySQL server password"), buffer, p.pwd);

	std::shared_ptr<CMySQLConnection> conn = std::make_shared<CMySQLConnection>();
	conn->Connect(p.server.c_str(), p.port, p.user.c_str(), \
					p.pwd.c_str(), p.database.c_str());
	return conn;
}

CMySQLConnectionPtr CMySQLConnectionFactory::createConnection(const CPropertiesFile &props, \
													const unsigned char(&key)[MAX_PWD_LEN]) {
	Tstring buffer;
	CMySQLConnectionFactory::Properties p = getProperties(props, buffer);

	Cryptor cryptor(key);
	auto pwd_e = getPropertyValue(props, _T("mysql_password_e"), \
								_T("MySQL server password encrypted"), buffer);
	auto pwd = cryptor.encrypt(pwd_e);
	UCS16_ToUTF8(pwd.c_str(), -1, p.pwd);

	std::shared_ptr<CMySQLConnection> conn = std::make_shared<CMySQLConnection>();
	conn->Connect(p.server.c_str(), p.port, p.user.c_str(), \
					p.pwd.c_str(), p.database.c_str());
	return conn;
}

CMySQLConnectionFactory::~CMySQLConnectionFactory() { }

#include <basic/tstring.h>
#include <basic/PropertiesFile.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "ParametersManager.h"

CParametersManager CParametersManager::inst;

CParametersManager::CParametersManager() : id_user(-1) { }

void CParametersManager::getParamsValuesFromFile(CPropertiesFile *props, \
												std::shared_ptr<IDbConnection> conn) {

	assert(props);
	Tstring buffer;

	const Tchar *user_name = props->getStringProperty(_T("user_name"), buffer);
	if (!user_name || (user_name && user_name[0] == _T('\0')))
		throw XException(0, _T("Параметр 'user_name' відсутній у config.ini"));

	auto stmt = conn->PrepareQuery("SELECT id_user FROM users WHERE user_name = ?");
	stmt->bindValue(0, user_name);
	auto rs = stmt->exec();

	if (rs->empty()) {
		XException e(0, _T("Користувач '"));
		e << user_name << _T("' відсутній в БД. Перевірте config.ini");
		throw e;
	}
	rs->gotoRecord(0);

	bool is_null;
	this->id_user = rs->getInt(0, is_null);
	assert(!is_null);
}

void CParametersManager::init(CPropertiesFile *props, std::shared_ptr<IDbConnection> conn) {

	if(!inst.areParamsInitilized())
		inst.getParamsValuesFromFile(props, conn);
	else
		throw XException(0, _T("Клас CParametersManager уже було ініціалізовано"));
}

CParametersManager::~CParametersManager() { }

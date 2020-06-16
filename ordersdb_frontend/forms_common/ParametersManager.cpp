#include "ParametersManager.h"
#include <basic/XString.h>
#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <basic/PropertiesFile.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "AutorizationManager.h"

CParametersManager CParametersManager::inst;

CParametersManager::CParametersManager() : id_user(-1), default_center(-1) { }

void CParametersManager::getParamsValuesFromFile(CPropertiesFile *props, \
												std::shared_ptr<IDbConnection> conn) {
	
	assert(props);
	assert(conn);
	Tstring buffer;

	determineUserAndGroups(props, conn, buffer);

	auto str_date = props->getStringProperty(_T("initial_order_date"), buffer);
	if (!str_date)
		throw XException(0, \
			_T("Параметр початкової дати 'initial_order_date' відсутній у config.ini"));

	if (!initial_order_date.setDateGerman(str_date)) {
		XException e(0, _T("Невірний формат початкової дати: "));
		e << str_date;
		throw e;
	}
	if (initial_order_date > CDate::now()) {
		XException e(0, _T("Початкова дата в майбутньому: "));
		e << str_date;
		throw e;
	}
	initial_order_date.toStringSQL(date_buffer);
	initial_order_date.toStringGerman(date_buffer_w);
}

void CParametersManager::determineUserAndGroups(CPropertiesFile *props, \
												std::shared_ptr<IDbConnection> conn, \
												Tstring &buffer) {

	assert(props);
	assert(conn);

	const Tchar *p_user_name = props->getStringProperty(_T("user_name"), buffer);
	if (!p_user_name || (p_user_name && p_user_name[0] == _T('\0')))
		throw XException(0, _T("Параметр 'user_name' відсутній у config.ini"));
	Tstring user_name = p_user_name;

	const Tchar *pwd = props->getStringProperty(_T("password"), buffer);
	if (!pwd || (pwd && pwd[0] == _T('\0')))
		throw XException(0, _T("Параметр 'password' відсутній у config.ini"));

	auto stmt = conn->PrepareQuery("SELECT id_user, id_center FROM users WHERE user_name = ?");
	stmt->bindValue(0, user_name.c_str());
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

	this->default_center = rs->getInt(1, is_null);
	assert(!is_null);
	if (default_center < 1 || default_center > 5) {
		XException e(0, _T("Невірний номер центру: "));
		e << default_center;
		throw e;
	}
	
	CAutorizationManager auth_mgr(conn);
	auth_mgr.autorize(id_user, user_name.c_str(), ImmutableString<Tchar>(pwd, Tstrlen(pwd)));

	stmt = conn->PrepareQuery("SELECT gr.group_name FROM groups gr INNER JOIN users_groups ug ON gr.id_group = ug.id_group WHERE ug.id_user = ? ORDER BY 1");
	stmt->bindValue(0, id_user);
	rs = stmt->exec();

	size_t groups_count = rs->getRecordsCount();
	if (!groups_count) {
		XException e(0, _T("Неможливо визначити список груп для користувача '"));
		e << user_name << _T("'");
		throw e;
	}

	for (size_t i = 0; i < groups_count; ++i) {
		rs->gotoRecord(i);
		groups.emplace_back(rs->getString(0));
	}
}

void CParametersManager::init(CPropertiesFile *props, std::shared_ptr<IDbConnection> conn) {

	if (!inst.areParamsInitilized())
		inst.getParamsValuesFromFile(props, conn);
	else
		throw XException(0, _T("Клас CParametersManager уже було ініціалізовано"));
}

std::string CParametersManager::getInitialFilteringStr() const {
	
	char int_buffer[10];

	std::string initial_flt = "a.id_center_legalaid = ";
	XConv::ToString(default_center, int_buffer);
	initial_flt += int_buffer;
	initial_flt += " AND a.order_date >= '";
	initial_flt += date_buffer;
	initial_flt += "'";

	return std::move(initial_flt);
}

const std::string &CParametersManager::getCenterFilteringStr() const {

	if (!center_flt_str.empty()) return center_flt_str;

	XString<char> str;

	if (default_center == 1) {
		UCS16_ToUTF8(L"РЦ", 2, str);

		center_flt_str = "a.zone = '";
		center_flt_str += str.c_str();
		center_flt_str += "'";
	}
	else {
		str.resize(10);
		XConv::ToString(default_center, &str[0]);

		center_flt_str = "a.id_center_legalaid = ";
		center_flt_str += str.c_str();
	}

	return center_flt_str;
}

CParametersManager::~CParametersManager() { }

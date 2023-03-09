#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "PermissionsManager.h"

CPermissionsManagerException::CPermissionsManagerException(const int err_code, \
													const Tchar *err_descr) : \
													XException(err_code, err_descr) { }

CPermissionsManagerException::CPermissionsManagerException(const CPermissionsManagerException &obj) : \
													XException(obj) { }

CPermissionsManagerException::~CPermissionsManagerException() { }

//*****************************************************

CPermissionsManager::CPermissionsManager() : admins(false), orders_inserters(false), \
											orders_deleters(false), payments_inserters(false), \
											payments_deleters(false) { }

CPermissionsManager::CPermissionsManager(std::shared_ptr<IDbConnection> conn, \
										const int id_user, \
										const Tstring &user_name) : \
								admins(false), local_admins(false), \
								orders_inserters(false), \
								orders_deleters(false), payments_deleters(false), \
								payments_inserters(false) {

	init(conn, id_user, user_name);
}

void CPermissionsManager::init(std::shared_ptr<IDbConnection> conn, \
								const int id_user, const Tstring &user_name) {

	auto stmt = conn->PrepareQuery("SELECT gr.group_name FROM groups gr INNER JOIN users_groups ug ON gr.id_group = ug.id_group WHERE ug.id_user = ? ORDER BY 1");
	stmt->bindValue(0, id_user);
	auto rs = stmt->exec();

	size_t groups_count = rs->getRecordsCount();
	if (!groups_count) {
		CPermissionsManagerException e(CPermissionsManagerException::E_GROUPS, \
			_T("Неможливо визначити список груп для користувача '"));
		e << user_name << _T("'");
		throw e;
	}

	for (size_t i = 0; i < groups_count; ++i) {
		rs->gotoRecord(i);
		const char *group_name = rs->getString(0);

		assert(group_name);
		if (!strcmp(group_name, "Administrators"))
			admins = true;
		else
			if (!strcmp(group_name, "Local admins"))
				local_admins = true;
			else
				if (!strcmp(group_name, "Orders inserters"))
					orders_inserters = true;
				else
					if (!strcmp(group_name, "Orders deleters"))
						orders_deleters = true;
					else
						if (!strcmp(group_name, "Payments inserters"))
							payments_inserters = true;
						else
							if (!strcmp(group_name, "Payments deleters"))
								payments_deleters = true;
	}
}

CPermissionsManager::~CPermissionsManager() { }

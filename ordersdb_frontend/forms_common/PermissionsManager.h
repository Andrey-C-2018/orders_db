#pragma once
#include <memory>
#include <basic/Exception.h>

class CPermissionsManagerException : public XException {
public:
	enum {
		E_GROUPS = 1
	};
	CPermissionsManagerException(const int err_code, const Tchar *err_descr);
	CPermissionsManagerException(const CPermissionsManagerException &obj);
	CPermissionsManagerException(CPermissionsManagerException &&obj) = default;
	~CPermissionsManagerException();
};

struct IDbConnection;

class CPermissionsManager {

	bool admins, orders_inserters, orders_deleters, accountants;
public:
	CPermissionsManager();
	CPermissionsManager(std::shared_ptr<IDbConnection> conn, \
						const int id_user, const Tstring &user_name);

	CPermissionsManager(const CPermissionsManager &obj) = default;
	CPermissionsManager(CPermissionsManager &&obj) = default;
	CPermissionsManager &operator=(const CPermissionsManager &obj) = default;
	CPermissionsManager &operator=(CPermissionsManager &&obj) = default;

	void init(std::shared_ptr<IDbConnection> conn, \
				const int id_user, const Tstring &user_name);

	inline bool isAdmin() const { return admins; }
	inline bool isOrdersInserter() const { return orders_inserters; }
	inline bool isOrdersDeleter() const { return orders_deleters; }
	inline bool isAccountant() const { return accountants; }

	~CPermissionsManager();
};


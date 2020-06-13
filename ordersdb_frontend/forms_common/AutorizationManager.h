#pragma once
#include <memory>
#include <basic/tstring.h>
#include <basic/ImmutableString.h>
#include <basic/Exception.h>

class CAutorizationManagerException : public XException {
public:
	enum {
		E_AUTORIZATION = 1
	};
	CAutorizationManagerException(const int err_code, const Tchar *err_descr);
	CAutorizationManagerException(const CAutorizationManagerException &obj);
	CAutorizationManagerException(CAutorizationManagerException &&obj) = default;
	~CAutorizationManagerException();
};

struct IDbConnection;

class CAutorizationManager {
	std::shared_ptr<IDbConnection> conn;

public:
	CAutorizationManager(std::shared_ptr<IDbConnection> conn_);

	CAutorizationManager(const CAutorizationManager &obj) = default;
	CAutorizationManager(CAutorizationManager &&obj) = default;
	CAutorizationManager &operator=(const CAutorizationManager &obj) = default;
	CAutorizationManager &operator=(CAutorizationManager &&obj) = default;

	void autorize(const int id_user, const Tchar *user_name, \
					ImmutableString<Tchar> pwd);
	
	virtual ~CAutorizationManager();
};


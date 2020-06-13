#include <basic/XString.h>
#include <basic/XConv.h>
#include <basic/TextConv.h>
#include <db/IDbConnection.h>
#include <db/IDbResultSet.h>
#include "AutorizationManager.h"

CAutorizationManagerException::CAutorizationManagerException(const int err_code, \
								const Tchar *err_descr) : \
								XException(err_code, err_descr) { }

CAutorizationManagerException::CAutorizationManagerException(const CAutorizationManagerException &obj) : \
								XException(obj) { }

CAutorizationManagerException::~CAutorizationManagerException() { }

//*****************************************************

CAutorizationManager::CAutorizationManager(std::shared_ptr<IDbConnection> conn_) : \
									conn(conn_) { }

void CAutorizationManager::autorize(const int id_user, const Tchar *user_name, \
									ImmutableString<Tchar> pwd) {

	assert(!pwd.isNull());
	assert(user_name);
	XString<char> str;

	std::string query = "SELECT id_user FROM users WHERE id_user = ";
	str.resize(10);
	query += XConv::ToString(id_user, &str[0]);
	query += " AND pwd_hash = SHA1('";
	UCS16_ToUTF8(pwd.str, (int)pwd.size, str);
	query += str.c_str();
	query += "')";

	auto rs = conn->ExecQuery(query.c_str());
	size_t size = rs->getRecordsCount();

	if (!size) {
		CAutorizationManagerException e(CAutorizationManagerException::E_AUTORIZATION, \
										_T("Користувач або пароль невірні: "));
		e << user_name << _T(", ") << pwd.str;
		throw e;
	}
}

CAutorizationManager::~CAutorizationManager() { }

#pragma once
#include <memory>

struct IDbConnection;

void setLastChangedUser();

template <class TDbTable> \
inline std::shared_ptr<TDbTable> getDbTablePtr(std::weak_ptr<TDbTable> &db_table) {

	auto ptr = db_table.lock();
	if (!ptr)
		throw XException(0, _T("DbTableEvtHandler: the DbTable pointer is NULL"));

	return std::move(ptr);
}

std::string getFormVersion(std::shared_ptr<IDbConnection>, const char *form_param);
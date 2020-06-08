#include <basic/XConv.h>
#include <basic/ImmutableString.h>
#include <db_ext/DependentTableUpdater.h>
#include "ParametersManager.h"
#include "CommonRoutines.h"

void setLastChangedUser() {

	std::string last_user_query = "d.id_user = ";
	int id_user = CParametersManager::getInstance().getIdUser();
	assert(id_user != -1);
	char buffer[20];
	XConv::ToString(id_user, buffer);
	last_user_query += buffer;

	ImmutableString<char> dep_table_user_str(last_user_query.c_str(), last_user_query.size());
	CDependentTableUpdater::setQueryConstantModifier(dep_table_user_str);
	ImmutableString<char> user_str(last_user_query.c_str() + 2, last_user_query.size() - 2);
	CMetaInfo::setQueryConstantModifier(user_str);
}
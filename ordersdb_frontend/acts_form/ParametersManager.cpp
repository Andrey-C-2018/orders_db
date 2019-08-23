#include <basic/tstring.h>
#include <basic/PropertiesFile.h>
#include "ParametersManager.h"

CParametersManager CParametersManager::inst;

CParametersManager::CParametersManager() : id_user(-1) { }

void CParametersManager::getParamsValuesFromFile(CPropertiesFile *props) {

	assert(props);
	Tstring buffer;

	bool not_found;
	int id_user = props->getIntProperty(_T("id_user"), buffer, not_found);
	if (not_found) 
		throw XException(0, _T("Параметр 'id_user' відсутній у config.ini"));
	this->id_user = id_user;
}

void CParametersManager::init(CPropertiesFile *props) {

	if(!inst.areParamsInitilized())
		inst.getParamsValuesFromFile(props);
	else
		throw XException(0, _T("Клас CParametersManager уже було ініціалізовано"));
}



CParametersManager::~CParametersManager() { }

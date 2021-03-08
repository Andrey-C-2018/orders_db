#include "FlexiblePropepties.h"
#include <basic/FileSystem.h>
#include <basic/Process.h>
#include <basic/Exception.h>
#include <basic/PropertiesFile.h>

CFlexiblePropepties::CFlexiblePropepties() { }

void CFlexiblePropepties::open() {

	assert(!per_machine && !per_user);
	if (checkFileExists("config.ini"))
		per_machine = std::make_unique<CPropertiesFile>("config.ini");

	std::string user_home_dir;
	getUserHomeDir(user_home_dir);

	assert(!user_home_dir.empty());
	user_home_dir += "\\.orders_frontend\\config.ini";
	if (checkFileExists(user_home_dir.c_str()))
		per_user = std::make_unique<CPropertiesFile>(user_home_dir.c_str());

	if (!per_machine && !per_user)
		throw XException(0, _T("Не знайдено жодного файлу config.ini"));
}

const Tchar *CFlexiblePropepties::getStringProperty(const Tchar *name, Tstring &buffer) const {

	const Tchar *value = nullptr;
	if(per_user)
		value = per_user->getStringProperty(name, buffer);
	if(per_machine && (!per_user || !value))
		value = per_machine->getStringProperty(name, buffer);

	return value;
}

const Tchar *CFlexiblePropepties::getStringPropertyThreadUnsafe(const Tchar *name) {

	const Tchar *value = nullptr;
	if (per_user)
		value = per_user->getStringPropertyThreadUnsafe(name);
	if (per_machine && (!per_user || !value))
		value = per_machine->getStringPropertyThreadUnsafe(name);

	return value;
}

int CFlexiblePropepties::getIntProperty(const Tchar *name, Tstring &buffer) const {

	int value = -1;
	if (per_user) {
		try {
			value = per_user->getIntProperty(name, buffer);
		}
		catch (CPropertiesFileException &e) {
			if (per_machine && e.GetErrorCode() == CPropertiesFileException::E_PROPERTY_NOT_FOUND)
				value = per_machine->getIntProperty(name, buffer);
			else
				throw;
		}
	}
	else 
		if(per_machine)
			value = per_machine->getIntProperty(name, buffer);

	return value;
}

int CFlexiblePropepties::getIntProperty(const Tchar *name, Tstring &buffer,
										bool &not_found) const {

	int value = -1;
	if (per_user)
		value = per_user->getIntProperty(name, buffer, not_found);
	if (per_machine && (!per_user || not_found))
		value = per_machine->getIntProperty(name, buffer, not_found);

	return value;
}

CFlexiblePropepties::~CFlexiblePropepties() { }

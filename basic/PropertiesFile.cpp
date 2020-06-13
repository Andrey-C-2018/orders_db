#include <codecvt>
#include <algorithm>
#include "locale_init.h"
#include "PropertiesFile.h"
#include "XConv.h"

CPropertiesFileException::CPropertiesFileException(const int err_code, \
													const Tchar *err_descr) : \
													XException(err_code, err_descr) { }

CPropertiesFileException::CPropertiesFileException(const int err_code, \
													const Tchar *err_descr, \
													const Tchar *prop_name_) : \
													XException(err_code, err_descr), \
													prop_name(prop_name_) { }

CPropertiesFileException::CPropertiesFileException(const CPropertiesFileException &obj) : \
													XException(obj) { }

CPropertiesFileException::~CPropertiesFileException() throw() { }

//****************************************************************************

CPropertiesFile::CPropertiesFile(){ }

CPropertiesFile::CPropertiesFile(const char *path) {

	open(path);
}

void CPropertiesFile::open(const char *path) {

	f.open(path, std::ios::in);
	if (!f.is_open())
		throw CPropertiesFileException(CPropertiesFileException::E_FILE_NOT_FOUND, \
			_T("Config file cannot be opened"));

	f.unsetf(std::ios::skipws);
	std::locale loc(getOS_Locale());
	f.imbue(loc);
}

const Tchar *CPropertiesFile::getStringProperty(const Tchar *name, Tstring &buffer) const {

	assert(name);

	if (!f.is_open())
		throw CPropertiesFileException(CPropertiesFileException::E_FILE_NOT_OPENED, \
			_T("The config file is not yet opened"));
	if (!name || (name && *name == _T('\0')))
		throw CPropertiesFileException(CPropertiesFileException::E_WRONG_NAME, \
			_T("Property name cannot be NULL or empty"));

	bool found = false;
	size_t name_size = Tstrlen(name);
	while (!found && !f.eof()) {
		std::getline(f, buffer);
		size_t size = std::min(buffer.size(), name_size);

		found = !Tstrncmp(buffer.c_str(), name, size);
		found = found && (buffer.size() > name_size) && buffer[name_size] == _T('=');
	}
	if (!found && f.eof()) {
		f.clear();
		f.seekg(0);

		while (!found && !f.eof()) {
			std::getline(f, buffer);
			size_t size = std::min(buffer.size(), name_size);

			found = !Tstrncmp(buffer.c_str(), name, size);
			found = found && (buffer.size() > name_size) && buffer[name_size] == _T('=');
		}
	}

	if (found) 
		return buffer.c_str() + name_size + 1;
	
	return nullptr;
}

const Tchar *CPropertiesFile::getStringPropertyThreadUnsafe(const Tchar *name){

	return getStringProperty(name, this->buffer);
}

int CPropertiesFile::getIntProperty(const Tchar *name, Tstring &buffer) const {

	const Tchar *p = getStringProperty(name, buffer);
	if (!p) {
		CPropertiesFileException e(CPropertiesFileException::E_PROPERTY_NOT_FOUND, \
									_T("Property name was not found: '"), name);
		e << name << "'";
		throw e;
	}
	int error = 0;
	int value = XConv::ToInt(p, error);

	if (error) {
		CPropertiesFileException e(CPropertiesFileException::E_WRONG_VALUE, \
									_T("Invalid integer value: '"), name);
		e << p << _T("' for property name '" << name << "'");
		throw e;
	}
	return value;
}

int CPropertiesFile::getIntProperty(const Tchar *name, Tstring &buffer, \
									bool &not_found) const {

	const Tchar *p = getStringProperty(name, buffer);
	if (!p) {
		not_found = true;
		return 0;
	}
	not_found = false;

	int error = 0;
	int value = XConv::ToInt(p, error);

	if (error) {
		CPropertiesFileException e(CPropertiesFileException::E_WRONG_VALUE, \
			_T("Invalid integer value: '"), name);
		e << p << _T("' for property name '" << name << "'");
		throw e;
	}
	return value;
}

CPropertiesFile::~CPropertiesFile(){

	if(f.is_open()) f.close();
}

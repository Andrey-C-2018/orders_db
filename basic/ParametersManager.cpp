#include <codecvt>
#include "PropertiesFile.h"

CPropertiesFileException::CPropertiesFileException(const int err_code, \
													const Tchar *err_descr) : \
													XException(err_code, err_descr) { }

CPropertiesFileException::CPropertiesFileException(const CPropertiesFileException &obj) : \
													XException(obj) { }

CPropertiesFileException::~CPropertiesFileException() throw() { }

//****************************************************************************

CPropertiesFile::CPropertiesFile(){ }

void CPropertiesFile::Open(const char *path) {

	f.open(path, std::ios::in);
	if (!f.is_open())
		throw CPropertiesFileException(CPropertiesFileException::E_FILE_NOT_FOUND, \
			_T("Config file cannot be opened"));

	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	f.imbue(loc);
}

void CPropertiesFile::SetProperty(const Tchar *name, const Tchar *value){ }

const Tchar *CPropertiesFile::GetProperty(const Tchar *name){

	if(!f.is_open())
		throw CPropertiesFileException(CPropertiesFileException::E_FILE_NOT_OPENED, \
								_T("The config file is not yet opened"));
	if (!name || (name && *name == _T('\0')))
		throw CPropertiesFileException(CPropertiesFileException::E_WRONG_NAME, \
								_T("Property name cannot be NULL or empty"));

	Tstring::size_type pos = Tstring::npos;
	while(pos == Tstring::npos && !f.eof()){
		std::getline(f, buffer);
		pos = buffer.find(name);
	}
	if (pos == Tstring::npos && f.eof()) {
		f.clear();
		f.seekg(0);

		while (pos == Tstring::npos && !f.eof()) {
			std::getline(f, buffer);
			pos = buffer.find(name);
		}
	}

	if(pos != Tstring::npos && pos == 0){
		Tstring::size_type pos_value = buffer.find_first_of(_T('='), pos + Tstrlen(name));
		if (pos_value == Tstring::npos) {
			CPropertiesFileException e(CPropertiesFileException::E_WRONG_FILE_FORMAT,\
									_T("Invalid config file format: property name - "));
			e << name;
			throw e;
		}
		else{
			const Tchar *p = buffer.c_str();
			return p + pos_value + 1;
		}
	}
	return nullptr;
}

CPropertiesFile::~CPropertiesFile(){

	if(f.is_open()) f.close();
}

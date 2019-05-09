#pragma once
#include "tstream.h"
#include "Exception.h"

class CPropertiesFileException : public XException {
public:
	enum {
		E_FILE_NOT_FOUND = 1, \
		E_FILE_NOT_OPENED = 2, \
		E_WRONG_NAME = 3, \
		E_WRONG_FILE_FORMAT = 4
	};
	CPropertiesFileException(const int err_code, const Tchar *err_descr);
	CPropertiesFileException(const CPropertiesFileException &obj);
	CPropertiesFileException(CPropertiesFileException &&obj) = default;
	~CPropertiesFileException() throw();
};

class CPropertiesFile{
	Tfstream f;
	Tstring buffer;
public:
	CPropertiesFile();
	CPropertiesFile(const char *path);
	void open(const char *path);
	const Tchar *getStringProperty(const Tchar *name);
	virtual ~CPropertiesFile();
};


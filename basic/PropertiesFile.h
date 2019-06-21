#pragma once
#include "tstream.h"
#include "Exception.h"

class BASIC_DLL_EXPORT CPropertiesFileException : public XException {
public:
	enum {
		E_FILE_NOT_FOUND = 1, \
		E_FILE_NOT_OPENED = 2, \
		E_WRONG_NAME = 3, \
		E_WRONG_FILE_FORMAT = 4, \
		E_WRONG_VALUE = 5
	};
	CPropertiesFileException(const int err_code, const Tchar *err_descr);
	CPropertiesFileException(const CPropertiesFileException &obj);
	CPropertiesFileException(CPropertiesFileException &&obj) = default;
	~CPropertiesFileException() throw();
};

class BASIC_DLL_EXPORT CPropertiesFile{
	mutable Tfstream f;
	Tstring buffer;

public:
	CPropertiesFile();
	CPropertiesFile(const char *path);
	void open(const char *path);
	const Tchar *getStringProperty(const Tchar *name, Tstring &buffer) const;
	const Tchar *getStringPropertyThreadUnsafe(const Tchar *name);
	int getIntProperty(const Tchar *name, Tstring &buffer) const;
	virtual ~CPropertiesFile();
};

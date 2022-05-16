#pragma once
#include "tfstream.h"
#include "Exception.h"
#include "IProperties.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

class BASIC_DLL_EXPORT CPropertiesFileException : public XException {
	Tstring prop_name;
public:
	enum {
		E_FILE_NOT_FOUND = 1, \
		E_FILE_NOT_OPENED = 2, \
		E_WRONG_NAME = 3, \
		E_WRONG_VALUE = 4, \
		E_PROPERTY_NOT_FOUND = 5
	};
	CPropertiesFileException(const int err_code, const Tchar *err_descr);
	CPropertiesFileException(const int err_code, const Tchar *err_descr, \
								const Tchar *prop_name_);
	CPropertiesFileException(const CPropertiesFileException &obj);
	CPropertiesFileException(CPropertiesFileException &&obj) = default;

	inline const Tchar *getPropertyName() const { return prop_name.c_str(); }

	~CPropertiesFileException() throw();
};

class BASIC_DLL_EXPORT CPropertiesFile : public IProperties {
	mutable Tfstream f;
	Tstring buffer;

public:
	CPropertiesFile();
	CPropertiesFile(const char *path);

	CPropertiesFile(const CPropertiesFile &obj) = delete;
	CPropertiesFile(CPropertiesFile &&obj) = default;
	CPropertiesFile &operator=(const CPropertiesFile &obj) = delete;
	CPropertiesFile &operator=(CPropertiesFile &&obj) = default;

	void open(const char *path);
	const Tchar *getStringProperty(const Tchar *name, Tstring &buffer) const override;
	const Tchar *getStringPropertyThreadUnsafe(const Tchar *name) override;
	int getIntProperty(const Tchar *name, Tstring &buffer) const override;
	int getIntProperty(const Tchar *name, Tstring &buffer, bool &not_found) const override;

	virtual ~CPropertiesFile();
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

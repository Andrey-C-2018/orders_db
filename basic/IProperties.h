#pragma once
#include "tstring.h"

struct IProperties {

	virtual const Tchar *getStringProperty(const Tchar *name, Tstring &buffer) const = 0;
	virtual const Tchar *getStringPropertyThreadUnsafe(const Tchar *name) = 0;
	virtual int getIntProperty(const Tchar *name, Tstring &buffer) const = 0;
	virtual int getIntProperty(const Tchar *name, Tstring &buffer, bool &not_found) const = 0;
	
	virtual ~IProperties() { }
};
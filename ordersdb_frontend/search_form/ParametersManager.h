#pragma once
#include <date/Date.h>

class CPropertiesFile;

class CParametersManager {
	int default_center;
	CDate initial_order_date;
	char date_buffer[CDate::SQL_FORMAT_LEN];
public:
	CParametersManager();

	CParametersManager(const CParametersManager &obj) = default;
	CParametersManager(CParametersManager &&obj) = default;
	CParametersManager &operator=(const CParametersManager &obj) = default;
	CParametersManager &operator=(CParametersManager &&obj) = default;

	void Init(CPropertiesFile *props);
	inline int getDefaultCenter() const;
	inline const char *getInitialDate() const;

	virtual ~CParametersManager();
};

//*****************************************************

int CParametersManager::getDefaultCenter() const {

	return default_center;
}

const char *CParametersManager::getInitialDate() const {

	assert(date_buffer[0] != '\0');
	return date_buffer;
}
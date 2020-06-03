#pragma once
#include <date/Date.h>
#include <string>

class CPropertiesFile;

class CParametersManager {
	int default_center;
	CDate initial_order_date;
	char date_buffer[CDate::SQL_FORMAT_LEN + 1];
	wchar_t date_buffer_w[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CParametersManager();

	CParametersManager(const CParametersManager &obj) = default;
	CParametersManager(CParametersManager &&obj) = default;
	CParametersManager &operator=(const CParametersManager &obj) = default;
	CParametersManager &operator=(CParametersManager &&obj) = default;

	void Init(CPropertiesFile *props);
	inline int getDefaultCenter() const;
	inline const char *getInitialDate() const;
	inline const wchar_t *getInitialDateW() const;
	std::string getInitialFilteringStr() const;

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

const wchar_t *CParametersManager::getInitialDateW() const {

	assert(date_buffer_w[0] != '\0');
	return date_buffer_w;
}

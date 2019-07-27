#pragma once
#include <date/Date.h>

class CPropertiesFile;

class CParametersManager {
	bool full_matching;
	bool only_unpaid_acts;
	bool check_processed;
	bool cached_acts_names;

	CDate initial_acts_date;
	char date_buffer[CDate::SQL_FORMAT_LEN];
public:
	CParametersManager(CPropertiesFile *props);

	CParametersManager(const CParametersManager &obj) = default;
	CParametersManager(CParametersManager &&obj) = default;
	CParametersManager &operator=(const CParametersManager &obj) = default;
	CParametersManager &operator=(CParametersManager &&obj) = default;

	inline bool processOnlyUnpaidActs() const;
	inline bool checkAlreadyProcessed() const;
	inline bool fullMatching() const;
	inline bool useCachedActsNamesIfPossible() const;
	inline const char *getInitialDate() const;

	virtual ~CParametersManager();
};

//*****************************************************

bool CParametersManager::processOnlyUnpaidActs() const {

	return !full_matching && only_unpaid_acts;
}

bool CParametersManager::checkAlreadyProcessed() const {

	return !full_matching && check_processed;
}

bool CParametersManager::fullMatching() const {

	return full_matching;
}

bool CParametersManager::useCachedActsNamesIfPossible() const {

	return full_matching || cached_acts_names;
}

const char *CParametersManager::getInitialDate() const {

	assert(date_buffer[0] != '\0');
	return date_buffer;
}
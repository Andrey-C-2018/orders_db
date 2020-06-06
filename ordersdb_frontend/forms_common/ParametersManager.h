#pragma once
#include <string>
#include <memory>
#include <date/Date.h>

class CPropertiesFile;
struct IDbConnection;

class CParametersManager {
	static CParametersManager inst;

	int id_user;
	int default_center;
	CDate initial_order_date;
	char date_buffer[CDate::SQL_FORMAT_LEN + 1];
	wchar_t date_buffer_w[CDate::GERMAN_FORMAT_LEN + 1];

	CParametersManager();
	void getParamsValuesFromFile(CPropertiesFile *props, \
									std::shared_ptr<IDbConnection> conn);
	inline bool areParamsInitilized() const { return id_user != -1; }

public:
	static void init(CPropertiesFile *props, std::shared_ptr<IDbConnection> conn);

	CParametersManager(const CParametersManager &obj) = delete;
	CParametersManager(CParametersManager &&obj) = delete;
	CParametersManager &operator=(const CParametersManager &obj) = delete;
	CParametersManager &operator=(CParametersManager &&obj) = delete;

	static inline const CParametersManager &getInstance() { return inst; }

	inline int getIdUser() const { return id_user; }
	inline int getDefaultCenter() const;
	inline const wchar_t *getInitialDateW() const;
	std::string getInitialFilteringStr() const;

	virtual ~CParametersManager();
};

//*****************************************************

int CParametersManager::getDefaultCenter() const {

	return default_center;
}

const wchar_t *CParametersManager::getInitialDateW() const {

	assert(date_buffer_w[0] != '\0');
	return date_buffer_w;
}

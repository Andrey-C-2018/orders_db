#pragma once
#include <vector>
#include <memory>
#include <basic/tstring.h>
#include <date/Date.h>
#include "PermissionsManager.h"

class CPropertiesFile;
struct IDbConnection;

class CParametersManager {
	static CParametersManager inst;

	int id_user;
	CPermissionsManager permissions_mgr;

	int default_center;
	mutable std::string center_flt_str;

	CDate initial_order_date;
	char date_buffer[CDate::SQL_FORMAT_LEN + 1];
	wchar_t date_buffer_w[CDate::GERMAN_FORMAT_LEN + 1];
	
	CParametersManager();
	void getParamsValuesFromFile(CPropertiesFile *props, \
									std::shared_ptr<IDbConnection> conn);
	void determineUserAndPermissions(CPropertiesFile *props, \
									std::shared_ptr<IDbConnection> conn, \
									Tstring &buffer);
	inline bool areParamsInitilized() const { return id_user != -1; }

public:
	static void init(CPropertiesFile *props, std::shared_ptr<IDbConnection> conn);

	CParametersManager(const CParametersManager &obj) = delete;
	CParametersManager(CParametersManager &&obj) = delete;
	CParametersManager &operator=(const CParametersManager &obj) = delete;
	CParametersManager &operator=(CParametersManager &&obj) = delete;

	static inline const CParametersManager &getInstance() { return inst; }

	inline int getIdUser() const { return id_user; }
	inline const CPermissionsManager &getPermissions() const { return permissions_mgr; }
	inline int getDefaultCenter() const;
	inline const wchar_t *getInitialDateW() const;
	std::string getInitialFilteringStr() const;
	const std::string &getCenterFilteringStr() const;

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

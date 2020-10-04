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

	std::string date_buffer;
	std::wstring date_buffer_w;
	
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

	inline const std::wstring &getInitialDateW() const;
	std::string getInitialFilteringStr() const;

	const std::string &getCenterFilteringStr() const;

	virtual ~CParametersManager();
};

//*****************************************************

int CParametersManager::getDefaultCenter() const {

	return default_center;
}

const std::wstring &CParametersManager::getInitialDateW() const {

	return date_buffer_w;
}

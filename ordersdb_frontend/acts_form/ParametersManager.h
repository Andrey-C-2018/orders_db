#pragma once

class CPropertiesFile;

class CParametersManager {
	int id_user;
	static CParametersManager inst;

	CParametersManager();
	void getParamsValuesFromFile(CPropertiesFile *props);
	inline bool areParamsInitilized() const { return id_user != -1; }
public:
	
	static void init(CPropertiesFile *props);

	CParametersManager(const CParametersManager &obj) = default;
	CParametersManager(CParametersManager &&obj) = default;
	CParametersManager &operator=(const CParametersManager &obj) = default;
	CParametersManager &operator=(CParametersManager &&obj) = default;

	static inline const CParametersManager &getInstance() { return inst; }
	inline int getIdUser() const { return id_user; }

	virtual ~CParametersManager();
};


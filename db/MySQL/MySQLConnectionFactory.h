#pragma once
#include "MySQLConnection.h"

class CPropertiesFile;

class CMySQLConnectionFactory {
public:
	CMySQLConnectionFactory();

	static std::shared_ptr<CMySQLConnection> createConnection(const CPropertiesFile &props);

	virtual ~CMySQLConnectionFactory();
};


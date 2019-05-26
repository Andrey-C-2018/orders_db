#pragma once
#include <memory>

struct IDbConnection;
class CPropertiesFile;

std::shared_ptr<IDbConnection> createMySQLConnection(const CPropertiesFile &props);
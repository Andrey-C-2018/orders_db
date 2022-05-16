#pragma once
#include "MySQLConnection.h"

struct IProperties;
typedef std::shared_ptr<CMySQLConnection> CMySQLConnectionPtr;

class DBLIB_DLL_EXPORT CMySQLConnectionFactory {
	struct Properties final {
		std::string server, user, pwd, database;
		int port;

		inline Properties() noexcept : port(0) { }
		inline Properties(const Properties &obj) = default;
		inline Properties(Properties &&obj) = default;
		inline Properties &operator=(const Properties &obj) = default;
		inline Properties &operator=(Properties &&obj) = default;
	};

	static Properties getProperties(const IProperties &props, \
									Tstring &buffer);
public:
	enum {
		MAX_PWD_LEN = 10
	};

	CMySQLConnectionFactory();

	static CMySQLConnectionPtr createConnection(const IProperties &props);
	static CMySQLConnectionPtr createConnection(const IProperties &props, \
												const unsigned char (&key)[MAX_PWD_LEN]);

	virtual ~CMySQLConnectionFactory();
};


#pragma once
#include <memory>

struct IDbConnection;

struct IDbInserter {
	virtual void prepare(std::shared_ptr<IDbConnection> conn) = 0;
	virtual bool insert() = 0;

	virtual ~IDbInserter() { }
};
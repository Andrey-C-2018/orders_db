#pragma once
#include <memory>

class CDbTable;

struct IDbCustomer{

	virtual void OnCustomerInitialized(std::shared_ptr<CDbTable> db_table) = 0;
	virtual ~IDbCustomer() { }
};
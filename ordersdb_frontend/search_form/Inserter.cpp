#include "Inserter.h"
#include <xwindows/platform_specific.h>

CInserter::CInserter() { }

void CInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	orders_inserter.prepare(conn);
	payments_inserter.prepare(conn);
}

bool CInserter::insert() {

	Tstring err_str;

	bool bound = orders_inserter.bind(err_str);
	if (bound && err_str.empty())
		bound = payments_inserter.bind(err_str);
	else {
		ErrorBox(err_str.c_str());
		return false;
	}
	
	bool ret_value = false;
	if (bound && err_str.empty()) {
		orders_inserter.insert();
		payments_inserter.insert();
		ret_value = true;
	}
	else 
		ErrorBox(err_str.c_str());

	return ret_value;
}

CInserter::~CInserter() { }

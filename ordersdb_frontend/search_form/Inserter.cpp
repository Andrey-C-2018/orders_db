#include "Inserter.h"
#include <xwindows/platform_specific.h>
#include <forms_common/ParametersManager.h>

CInserter::CInserter() : allow_orders(false), allow_payments(false) { }

void CInserter::evalPermissions() {

	const auto &params_manager = CParametersManager::getInstance();
	auto &perm_mgr = CParametersManager::getInstance().getPermissions();

	if (perm_mgr.isAdmin()) {

		allow_orders = true;
		allow_payments = true;
		orders_inserter.adminLogged(true);
	}
	else {
		allow_orders = perm_mgr.isOrdersInserter();
		allow_payments = allow_orders ? true : perm_mgr.isAccountant();
	}
}

void CInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	orders_inserter.prepare(conn);
	payments_inserter.prepare(conn);
}

bool CInserter::insert() {

	Tstring err_str;

	bool bound = allow_orders ? orders_inserter.bind(err_str) : true;
	if (bound && err_str.empty())
		bound = allow_payments ? payments_inserter.bind(err_str) : true;
	else {
		ErrorBox(err_str.c_str());
		return false;
	}
	
	bool ret_value = false;
	if (bound && err_str.empty()) {
		if(allow_orders) orders_inserter.insert();
		if(allow_payments) payments_inserter.insert();
		ret_value = allow_orders || allow_payments;
	}
	else 
		ErrorBox(err_str.c_str());

	return ret_value;
}

CInserter::~CInserter() { }

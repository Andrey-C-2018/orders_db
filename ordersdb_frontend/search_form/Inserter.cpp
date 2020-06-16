#include "Inserter.h"
#include <xwindows/platform_specific.h>
#include <forms_common/ParametersManager.h>

CInserter::CInserter() : allow_orders(false), allow_payments(false) { }

void CInserter::evalPermissions() {

	const auto &params_manager = CParametersManager::getInstance();
	auto groups = params_manager.getGroups();

	if (std::find(groups.cbegin(), groups.cend(), "Administrators") != \
		groups.cend()) {

		allow_orders = true;
		allow_payments = true;
		orders_inserter.adminLogged(true);
	}
	else {
		allow_orders = std::find(groups.cbegin(), groups.cend(), "Orders keepers") != \
			groups.cend();
		allow_payments = allow_orders ? true : \
			std::find(groups.cbegin(), groups.cend(), "Accountants") != \
			groups.cend();
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

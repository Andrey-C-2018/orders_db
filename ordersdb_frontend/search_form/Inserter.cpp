#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include <db/DbException.h>
#include <xwindows/platform_specific.h>
#include <forms_common/ParametersManager.h>
#include "Inserter.h"

CInserter::CInserter() : allow_orders(false), allow_payments(false) { }

void CInserter::evalPermissions() {

	const auto &params_manager = CParametersManager::getInstance();
	auto &perm_mgr = CParametersManager::getInstance().getPermissions();

	const bool admin = perm_mgr.isAdmin();
	const bool local_admin = perm_mgr.isLocalAdmin();
	allow_orders = admin || local_admin || perm_mgr.isOrdersInserter();
	allow_payments = admin || local_admin || perm_mgr.isPaymentsInserter();

	ins_orders.adminLogged(admin);
	ins_payments.includeOrdersProps(!allow_orders);

	ins_payments.adminLogged(admin);
}

void CInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	std::string query;
	if (allow_orders) {
		query = "call insertFull(";
		char ignore_existing_order = '0' + (char)((int)allow_payments);
		query += ignore_existing_order;
		query += ',';
							
		ins_orders.createBinders();
		ins_orders.getHelper().buildQuery(query);
		query += ',';
	}
	else 
		query = "call insertStage(";
	
	ins_payments.createBinders(conn);
	ins_payments.getHelper().buildQuery(query);
	query += ')';

	stmt = conn->PrepareQuery(query.c_str());
}

bool CInserter::insert() {

	Tstring err_str;
	bool success = true;
	size_t initial_param_no = 0;

	if (allow_orders) {
		success = ins_orders.getHelper().bind(stmt, 0, err_str);
		initial_param_no = ins_orders.getHelper().getBindersCount();
	}

	auto &pay_hlp = ins_payments.getHelper();
	success = success && pay_hlp.bind(stmt, initial_param_no, err_str);
	if (!success) {
		ErrorBox(err_str.c_str());
		return false;
	}

	assert(err_str.empty());
	try {
		stmt->execScalar();
	}
	catch (CDbException &e) {

		int err_no = e.GetErrorCode();
		if (err_no == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			if (!Tstrcmp(e.what(), _T("orders")))
				ins_orders.errMsgOnPrimKeyDuplicate(err_str);
			else {
				ins_payments.errMsgOnPrimKeyDuplicate(err_str);
				err_str += _T("\nДоручення: ");
				ins_orders.appendOrderInfo(err_str);
			}
			ErrorBox(err_str.c_str());
		}
		else if (err_no == CDbException::E_DB_NO_REF_IN_MASTER_TABLE) {

			assert(Tstrcmp(e.what(), _T("orders")) != 0);
			assert(!allow_orders);
			ins_payments.errMsgOnInvalidRef(err_str);
			err_str += _T("\nДоручення: ");
			ins_orders.appendOrderInfo(err_str);
			ErrorBox(err_str.c_str());
		}
		else throw;
	}

	return err_str.empty();
}

CInserter::~CInserter() { }

#include "DbTableDispatcher.h"
#include "IDbCustomer.h"

CDbTableDispatcherException::CDbTableDispatcherException(const int err_code, const Tchar *err_descr) : \
														XException(err_code, err_descr) { }

CDbTableDispatcherException::~CDbTableDispatcherException() { }

//****************************************************************************

typedef CDbTableDispatcher::id_type id_type;

CDbTableDispatcher::CDbTableDispatcher() { }

id_type CDbTableDispatcher::AddCustomer(std::shared_ptr<IDbCustomer> obj, \
										const id_type id_table) {
	CustomerIterator p_cust;
	TableIterator p_table;

	p_table = FindTable(id_table);
	
	p_cust = FindCustomer(obj);
	if (p_cust != customers.end()) {
		CDbTableDispatcherException e(CDbTableDispatcherException::E_CUSTOMER_IS_ALREADY_CONNECTED, \
									_T("the customer is already connected: id = "));
		e << p_cust->id;
		throw e;
	}

	id_type new_id = customers.size();
	customers.emplace_back(CCustomerRecord{ new_id, id_table, obj });
	std::sort(customers.begin(), customers.end());

	obj->OnCustomerInitialized(p_table->db_table);

	return new_id;
}

void CDbTableDispatcher::DelCustomer(const id_type id_customer) {
	CustomerIterator p_cust = FindCustomer(id_customer);

	customers.erase(p_cust);
}

std::shared_ptr<IDbCustomer> CDbTableDispatcher::GetCustomer(const id_type id) const {
	CustomerConstIterator p_cust = FindCustomer(id);

	return p_cust->customer;
}

id_type CDbTableDispatcher::AddDbTable(std::shared_ptr<CDbTable> table, const int attribs) {
	CDbTableRecord rec;
	
	rec.id = tables.size() + 1;
	rec.db_table = table;
	tables.emplace_back(rec);

	return rec.id;
}

CDbTableDispatcher::~CDbTableDispatcher(){ }

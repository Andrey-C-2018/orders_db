#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/Exception.h>
#include <db_ext/DbTable.h>
#include "IDbCustomer.h"

class CDbTableDispatcherException : public XException {
public:
	enum Errors {
		E_CUSTOMER_ISNT_CONNECTED = 2, \
		E_CUSTOMER_IS_ALREADY_CONNECTED = 3, \
		E_NO_SUCH_TABLE_ID = 5
	};

	CDbTableDispatcherException(const int err_code, const Tchar *err_descr);
	CDbTableDispatcherException(const CDbTableDispatcherException &obj) = default;
	CDbTableDispatcherException(CDbTableDispatcherException &&obj) = default;
	~CDbTableDispatcherException();
};

class CDbTableDispatcher final {

	struct CDbTableRecord {
		id_type id;
		std::shared_ptr<CDbTable> db_table;
	};

	struct CCustomerRecord {
		id_type id;
		id_type id_table;
		std::shared_ptr<IDbCustomer> customer;
	};

	std::vector<CDbTableRecord> tables;
	std::vector<CCustomerRecord> customers;

	typedef std::vector<CDbTableRecord>::iterator TableIterator;
	typedef std::vector<CCustomerRecord>::iterator CustomerIterator;
	typedef std::vector<CCustomerRecord>::const_iterator CustomerConstIterator;

	template <typename Iter> \
		static inline Iter FindCustomer(const id_type id, Iter p_begin, Iter p_end);
	inline CustomerIterator FindCustomer(const id_type id);
	inline CustomerConstIterator FindCustomer(const id_type id) const;
	inline CustomerIterator FindCustomer(std::shared_ptr<IDbCustomer> obj) noexcept;

public:
	typedef int id_type;

	CDbTableDispatcher();

	id_type AddDbTable(std::shared_ptr<CDbTable> table, const int attribs);

	id_type AddCustomer(std::shared_ptr<IDbCustomer> obj, const id_type id_query);
	void DelCustomer(const id_type id_customer);
	std::shared_ptr<IDbCustomer> GetCustomer(const id_type id) const;

	virtual ~CDbTableDispatcher();
};

//****************************************************************************

template <typename Iter> \
Iter CDbTableDispatcher::FindCustomer(const id_type id, Iter p_begin, Iter p_end){

	Iter p_cust;

	p_cust = std::find(p_begin, p_end, id);
	if (p_cust == customers.end()) {
		CDatabaseException e(CDatabaseException::E_CUSTOMER_ISNT_CONNECTED, \
			_T("no such customer: id = "));
		e << id;
		throw e;
	}
}

CDbTableDispatcher::CustomerIterator CDbTableDispatcher::FindCustomer(const id_type id) {

	return FindCustomer<CustomerIterator>(id, \
											customers.begin(), customers.end());
}

CDbTableDispatcher::CustomerConstIterator \
CDbTableDispatcher::FindCustomer(const id_type id) const {

	return FindCustomer<CustomerConstIterator>(id, \
											customers.cbegin(), customers.cend());
}

CDbTableDispatcher::CustomerIterator \
CDbTableDispatcher::FindCustomer(std::shared_ptr<IDbCustomer> obj) noexcept {
	CustomerIterator p_cust;

	p_cust = std::find_if(customers.begin(), customers.end(), \
		[&obj](const CCustomerRecord &rec) {
			
			return obj == rec.customer;
		});

	return p_cust;
}

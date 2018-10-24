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
public:
	typedef int id_type;
private:

	struct CDbTableRecord {
		id_type id;
		std::shared_ptr<CDbTable> db_table;

		inline bool operator==(const id_type id) const {

			return this->id == id;
		}
	};

	struct CCustomerRecord {
		id_type id;
		id_type id_table;
		std::shared_ptr<IDbCustomer> customer;

		inline bool operator==(const CCustomerRecord &obj) const {

			return id == obj.id;
		}
		inline bool operator==(const id_type id) const {

			return id == this->id;
		}
	};

	std::vector<CDbTableRecord> tables;
	std::vector<CCustomerRecord> customers;

	typedef std::vector<CDbTableRecord>::const_iterator TableConstIterator;
	typedef std::vector<CCustomerRecord>::iterator CustomerIterator;
	typedef std::vector<CCustomerRecord>::const_iterator CustomerConstIterator;

	inline CustomerConstIterator FindCustomer(const id_type id) const;
	inline CustomerConstIterator \
		FindCustomer(std::shared_ptr<IDbCustomer> obj) const noexcept;

	inline TableConstIterator FindTable(const id_type id) const;
public:
	CDbTableDispatcher();

	id_type AddDbTable(std::shared_ptr<CDbTable> table, const int attribs);

	id_type AddCustomer(std::shared_ptr<IDbCustomer> obj, const id_type id_query);
	void DelCustomer(const id_type id_customer);
	std::shared_ptr<IDbCustomer> GetCustomer(const id_type id) const;

	virtual ~CDbTableDispatcher();
};

//****************************************************************************

CDbTableDispatcher::CustomerConstIterator \
CDbTableDispatcher::FindCustomer(const id_type id) const{

	CustomerConstIterator p_cust = std::find(customers.cbegin(), customers.cend(), id);
	if (p_cust == customers.cend()) {
		CDbTableDispatcherException e(CDbTableDispatcherException::E_CUSTOMER_ISNT_CONNECTED, \
									_T("no such customer: id = "));
		e << id;
		throw e;
	}
	return p_cust;
}

CDbTableDispatcher::CustomerConstIterator \
	CDbTableDispatcher::FindCustomer(std::shared_ptr<IDbCustomer> obj) const noexcept {
	CustomerConstIterator p_cust;

	p_cust = std::find_if(customers.cbegin(), customers.cend(), \
		[&obj](const CCustomerRecord &rec) {

			return obj == rec.customer;
		});

	return p_cust;
}

CDbTableDispatcher::TableConstIterator \
	CDbTableDispatcher::FindTable(const id_type id) const {
	CDbTableDispatcher::TableConstIterator p_table;

	p_table = std::find(tables.cbegin(), tables.cend(), id);
	if (p_table == tables.cend()) {
		CDbTableDispatcherException e(CDbTableDispatcherException::E_NO_SUCH_TABLE_ID, \
										_T("no such table: id = "));
		e << id;
		throw e;
	}
	return p_table;
}

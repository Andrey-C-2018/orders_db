#pragma once
#include "OrdersInserter.h"
#include "PaymentsInserter.h"

class CInserter {
	bool allow_orders, allow_payments;
	COrdersInserter orders_inserter;
	CPaymentsInserter payments_inserter;

public:
	CInserter();
	void evalPermissions();

	CInserter(const CInserter &obj) = delete;
	CInserter(CInserter &&obj) = delete;
	CInserter &operator=(const CInserter &obj) = delete;
	CInserter &operator=(CInserter &&obj) = delete;

	inline COrdersInserter &getOrdersInserter() { return orders_inserter; }
	inline CPaymentsInserter &getPaymentsInserter() { return payments_inserter; }

	inline void SetCenterBox(CDbComboBox *center);
	inline void SetIdOrderWidget(XWidget *id_order);
	inline void SetOrderDateWidget(XWidget *order_date);

	void prepare(std::shared_ptr<IDbConnection> conn);
	bool insert();

	virtual ~CInserter();
};

//*****************************************************

void CInserter::SetCenterBox(CDbComboBox *center) {

	orders_inserter.SetCenterBox(center);
	payments_inserter.setCenterBinder(orders_inserter.getCenterBinder(), center);
}

void CInserter::SetIdOrderWidget(XWidget *id_order) {

	orders_inserter.SetIdOrderWidget(id_order);
	payments_inserter.setIdOrderBinder(orders_inserter.getIdOrderBinder());
}

void CInserter::SetOrderDateWidget(XWidget *order_date) {

	orders_inserter.SetOrderDateWidget(order_date);
	payments_inserter.setOrderDateBinder(orders_inserter.getOrderDateBinder());
}
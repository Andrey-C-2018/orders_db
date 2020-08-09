#pragma once
#include <db_ext/IDbInserter.h>
#include "OrdersInsertHelper.h"
#include "PaymentsInsertHelper.h"

struct IDbStatement;

class CInserter : public IDbInserter {
	bool allow_orders, allow_payments;

	COrdersInsertHelper ins_orders;
	CPaymentsInsertHelper ins_payments;
	std::shared_ptr<IDbStatement> stmt;

public:
	CInserter();
	void evalPermissions();

	CInserter(const CInserter &obj) = delete;
	CInserter(CInserter &&obj) = delete;
	CInserter &operator=(const CInserter &obj) = delete;
	CInserter &operator=(CInserter &&obj) = delete;

	inline COrdersInsertHelper &getOrdersInsertHelper() { return ins_orders; }
	inline CPaymentsInsertHelper &getPaymentsInsertHelper() { return ins_payments; }

	inline void SetCenterBox(CDbComboBox *center);
	inline void SetIdOrderWidget(XWidget *id_order);
	inline void SetOrderDateWidget(XWidget *order_date);

	void prepare(std::shared_ptr<IDbConnection> conn) override;
	bool insert() override;

	inline void resetControls();

	virtual ~CInserter();
};

//*****************************************************

void CInserter::SetCenterBox(CDbComboBox *center) {

	ins_orders.SetCenterBox(center);
	ins_payments.setCenterBinder(ins_orders.getCenterBinder(), center);
}

void CInserter::SetIdOrderWidget(XWidget *id_order) {

	ins_orders.SetIdOrderWidget(id_order);
	ins_payments.setIdOrderBinder(ins_orders.getIdOrderBinder());
}

void CInserter::SetOrderDateWidget(XWidget *order_date) {

	ins_orders.SetOrderDateWidget(order_date);
	ins_payments.setOrderDateBinder(ins_orders.getOrderDateBinder(), \
											order_date);
}

void CInserter::resetControls() {

	ins_orders.resetControls();
	ins_payments.resetControls();
}
#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class CPaymentsInserter : public CDbInserter {
	enum {
		FIELDS_COUNT = 35
	};
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IInsertBinder> center_binder;
	std::shared_ptr<IInsertBinder> id_order_binder;
	std::shared_ptr<IInsertBinder> order_date_binder;
	
	CDbComboBox *center;
	CDbComboBox *stage;
	XWidget *cycle;
	XWidget *article;
	XWidget *fee;
	XWidget *outgoings;
	CDbComboBox *informer;
	XWidget *id_act;
	XWidget *act_date;
	XWidget *act_reg_date;
	XWidget *payment_date;

public:
	CPaymentsInserter();

	CPaymentsInserter(const CPaymentsInserter &obj) = delete;
	CPaymentsInserter(CPaymentsInserter &&obj) = delete;
	CPaymentsInserter &operator=(const CPaymentsInserter &obj) = delete;
	CPaymentsInserter &operator=(CPaymentsInserter &&obj) = delete;

	inline void setCenterBinder(std::shared_ptr<IInsertBinder> binder);
	inline void setIdOrderBinder(std::shared_ptr<IInsertBinder> binder);
	inline void setOrderDateBinder(std::shared_ptr<IInsertBinder> binder);
	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~CPaymentsInserter();
};

//*****************************************************.

void CPaymentsInserter::setCenterBinder(std::shared_ptr<IInsertBinder> binder) {

	this->center_binder = binder;
}

void CPaymentsInserter::setIdOrderBinder(std::shared_ptr<IInsertBinder> binder) {

	this->id_order_binder = binder;
}

void CPaymentsInserter::setOrderDateBinder(std::shared_ptr<IInsertBinder> binder) {

	this->order_date_binder = binder;
}
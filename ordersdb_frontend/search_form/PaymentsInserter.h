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

	inline void setStageWidget(CDbComboBox *stage) { this->stage = stage; }
	inline void setCycleWidget(XWidget *cycle) { this->cycle = cycle; }
	inline void setArticleWidget(XWidget *article) { this->article = article; }
	inline void setFeeWidget(XWidget *fee) { this->fee = fee; }
	inline void setOutgoingsWidget(XWidget *outgoings) { this->outgoings = outgoings; }
	inline void setInformerWidget(CDbComboBox *informer) { this->informer = informer; }
	inline void setActWidget(XWidget *id_act) { this->id_act = id_act; }
	inline void setActDateWidget(XWidget *act_date) { this->act_date = act_date; }
	inline void setActRegDateWidget(XWidget *act_reg_date) { this->act_reg_date = act_reg_date; }
	inline void setPaymentDateWidget(XWidget *payment_date) { this->payment_date = payment_date; }

	inline void setCenterBinder(std::shared_ptr<IInsertBinder> binder, \
								CDbComboBox *center);
	inline void setIdOrderBinder(std::shared_ptr<IInsertBinder> binder);
	inline void setOrderDateBinder(std::shared_ptr<IInsertBinder> binder);
	void prepare(std::shared_ptr<IDbConnection> conn) override;
	bool insert() override;

	virtual ~CPaymentsInserter();
};

//*****************************************************.

void CPaymentsInserter::setCenterBinder(std::shared_ptr<IInsertBinder> binder, \
										CDbComboBox *center) {

	this->center_binder = binder;
	this->center = center;
}

void CPaymentsInserter::setIdOrderBinder(std::shared_ptr<IInsertBinder> binder) {

	this->id_order_binder = binder;
}

void CPaymentsInserter::setOrderDateBinder(std::shared_ptr<IInsertBinder> binder) {

	this->order_date_binder = binder;
}

#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class CPaymentsInserter : public CDbInserter {
	enum {
		FIELDS_COUNT = 37
	};
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IInsertBinder> center_binder;
	std::shared_ptr<IInsertBinder> id_order_binder;
	std::shared_ptr<IInsertBinder> order_date_binder;
	
	XWidget *order_date;

	CDbComboBox *stage;
	XWidget *cycle;
	XWidget *article;
	XWidget *fee;
	XWidget *outg_extra, *outg_post, *outg_daynight;
	CDbComboBox *informer;
	XWidget *id_act;
	XWidget *act_date;
	XWidget *act_reg_date;
	XWidget *payment_date;
	CDbComboBox *checker;

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
	inline void setOutgExtraWidget(XWidget *outg_extra) { this->outg_extra = outg_extra; }
	inline void setOutgPostWidget(XWidget *outg_post) { this->outg_post = outg_post; }
	inline void setOutgDayNightWidget(XWidget *outg_dn) { this->outg_daynight = outg_dn; }
	inline void setInformerWidget(CDbComboBox *informer) { this->informer = informer; }
	inline void setActWidget(XWidget *id_act) { this->id_act = id_act; }
	inline void setActDateWidget(XWidget *act_date) { this->act_date = act_date; }
	inline void setActRegDateWidget(XWidget *act_reg_date) { this->act_reg_date = act_reg_date; }
	inline void setPaymentDateWidget(XWidget *payment_date) { this->payment_date = payment_date; }
	inline void setCheckerWidget(CDbComboBox *checker) { this->checker = checker; }

	inline void setCenterBinder(std::shared_ptr<IInsertBinder> binder, \
								CDbComboBox *center);
	inline void setIdOrderBinder(std::shared_ptr<IInsertBinder> binder);
	inline void setOrderDateBinder(std::shared_ptr<IInsertBinder> binder, \
									XWidget *order_date);
	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~CPaymentsInserter();
};

//*****************************************************.

void CPaymentsInserter::setCenterBinder(std::shared_ptr<IInsertBinder> binder, \
										CDbComboBox *center) {

	this->center_binder = binder;
}

void CPaymentsInserter::setIdOrderBinder(std::shared_ptr<IInsertBinder> binder) {

	this->id_order_binder = binder;
}

void CPaymentsInserter::setOrderDateBinder(std::shared_ptr<IInsertBinder> binder, \
											XWidget *order_date) {

	this->order_date_binder = binder;
	this->order_date = order_date;
}

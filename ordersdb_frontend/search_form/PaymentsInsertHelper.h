#pragma once
#include <db_ext/DbInsertHelper.h>
#include <db_controls/InsertionBinders.h>
#include <db_controls/DbComboBox.h>

struct IDbConnection;

class CPaymentsInsertHelper {
	
	bool incl_order_props;
	bool admin_logged;
	std::shared_ptr<IInsertBinder> id_order_binder;
	std::shared_ptr<IInsertBinder> order_date_binder;
	
	CDbInsertHelper ins_helper;
	CDbComboBox *center;
	XWidget *order_date;

	CDbComboBox *stage;
	XWidget *cycle;
	XWidget *article;
	XWidget *fee;
	XWidget *outg_post, *outg_daynight;
	CDbComboBox *informer;
	XWidget *id_act;
	XComboBox *act_no;
	XWidget *act_date;
	XWidget *act_reg_date;
	XWidget *payment_date;
	CDbComboBox *checker;

	inline void init();
	inline void appendStageInfo(Tstring& str) const;

public:
	enum {
		FIELDS_COUNT = 20
	};

	CPaymentsInsertHelper();
	CPaymentsInsertHelper(const bool incl_order_props_);

	inline void includeOrdersProps(const bool incl_order_props_);

	CPaymentsInsertHelper(const CPaymentsInsertHelper &obj) = delete;
	CPaymentsInsertHelper(CPaymentsInsertHelper &&obj) = delete;
	CPaymentsInsertHelper &operator=(const CPaymentsInsertHelper &obj) = delete;
	CPaymentsInsertHelper &operator=(CPaymentsInsertHelper &&obj) = delete;

	inline void setStageWidget(CDbComboBox *stage) { this->stage = stage; }
	inline void setCycleWidget(XWidget *cycle) { this->cycle = cycle; }
	inline void setArticleWidget(XWidget *article) { this->article = article; }
	inline void setFeeWidget(XWidget *fee) { this->fee = fee; }
	inline void setOutgPostWidget(XWidget *outg_post) { this->outg_post = outg_post; }
	inline void setOutgDayNightWidget(XWidget *outg_dn) { this->outg_daynight = outg_dn; }
	inline void setInformerWidget(CDbComboBox *informer) { this->informer = informer; }
	inline void setActWidget(XWidget *id_act) { this->id_act = id_act; }
	inline void setActNoWidget(XComboBox *act_no) { this->act_no = act_no; }
	inline void setActDateWidget(XWidget *act_date) { this->act_date = act_date; }
	inline void setActRegDateWidget(XWidget *act_reg_date) { this->act_reg_date = act_reg_date; }
	inline void setPaymentDateWidget(XWidget *payment_date) { this->payment_date = payment_date; }
	inline void setCheckerWidget(CDbComboBox *checker) { this->checker = checker; }

	inline void setCenterWidget(CDbComboBox *center_);
	inline void setIdOrderBinder(std::shared_ptr<IInsertBinder> binder);
	inline void setOrderDateBinder(std::shared_ptr<IInsertBinder> binder, \
									XWidget *order_date);

	inline void adminLogged(const bool admin_logged_);
	void createBinders(std::shared_ptr<IDbConnection> conn);
	inline const CDbInsertHelper &getHelper() const { return ins_helper; }
	void errMsgOnPrimKeyDuplicate(Tstring &err_str) const;
	void errMsgOnInvalidRef(Tstring &err_str) const;

	void resetControls();

	virtual ~CPaymentsInsertHelper();
};

//*****************************************************

void CPaymentsInsertHelper::init() {

	order_date = nullptr;
	stage = nullptr;
	cycle = nullptr;
	article = nullptr;
	fee = nullptr;
	outg_post = nullptr;
	outg_daynight = nullptr;
	informer = nullptr;
	id_act = nullptr;
	act_no = nullptr;
	act_date = nullptr;
	act_reg_date = nullptr;
	payment_date = nullptr;
	checker = nullptr;
}

void CPaymentsInsertHelper::includeOrdersProps(const bool incl_order_props_) {

	this->incl_order_props = incl_order_props_;
	ins_helper.init(this->incl_order_props ? FIELDS_COUNT : \
												FIELDS_COUNT - 4);
}

void CPaymentsInsertHelper::adminLogged(const bool admin_logged_) {

	this->admin_logged = admin_logged_;
}

void CPaymentsInsertHelper::setCenterWidget(CDbComboBox *center_) {

	this->center = center_;
}

void CPaymentsInsertHelper::setIdOrderBinder(std::shared_ptr<IInsertBinder> binder) {

	this->id_order_binder = binder;
}

void CPaymentsInsertHelper::setOrderDateBinder(std::shared_ptr<IInsertBinder> binder, \
											XWidget *order_date) {

	this->order_date_binder = binder;
	this->order_date = order_date;
}

void CPaymentsInsertHelper::appendStageInfo(Tstring& str) const {

	str += stage->GetLabel();
	str += _T(", № розгляду: ");
	str += cycle->GetLabel();
	str += _T(", тип акту: ");
	str += act_no->GetLabel();
}

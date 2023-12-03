#pragma once
#include <db_ext/DbInsertHelper.h>
#include <db_controls/InsertionBinders.h>

class XComboBox;
class CDbTable;

class COrdersInsertHelper {

	CDbInsertHelper ins_helper;
	bool admin_logged;

	CDbComboBox *center;
	XWidget *id_order;
	XWidget *order_date;
	CDbComboBox *order_type;
	CDbComboBox *defender;
	XWidget *client;
	XWidget *bdate;

	std::shared_ptr<IInsertBinder> id_order_binder;
	std::shared_ptr<IInsertBinder> order_date_binder;

public:
	enum {
		FIELDS_COUNT = 9
	};

	COrdersInsertHelper();

	COrdersInsertHelper(const COrdersInsertHelper &obj) = delete;
	COrdersInsertHelper(COrdersInsertHelper &&obj) = delete;
	COrdersInsertHelper &operator=(const COrdersInsertHelper &obj) = delete;
	COrdersInsertHelper &operator=(COrdersInsertHelper &&obj) = delete;

	inline void adminLogged(const bool admin_logged);

	void SetCenterBox(CDbComboBox *center);
	void SetIdOrderWidget(XWidget *id_order);
	void SetOrderDateWidget(XWidget *order_date);
	inline void SetOrderTypeWidget(CDbComboBox *order_type);
	inline void SetDefenderWidget(CDbComboBox *defender);
	inline void SetClientWidget(XWidget *client);
	inline void SetClientBirthDateWidget(XWidget *bdate);

	inline std::shared_ptr<IInsertBinder> getIdOrderBinder();
	inline std::shared_ptr<IInsertBinder> getOrderDateBinder();

	void createBinders();
	inline const CDbInsertHelper &getHelper() const { return ins_helper; }
	inline void errMsgOnPrimKeyDuplicate(Tstring &err_str) const;
	void appendOrderInfo(Tstring &err_str) const;

	void resetControls();
	
	virtual ~COrdersInsertHelper();
};

//*****************************************************

void COrdersInsertHelper::errMsgOnPrimKeyDuplicate(Tstring &err_str) const {

	err_str = _T("Доручення уже існує: ");
	appendOrderInfo(err_str);
	err_str += _T("\nЯкщо необхідно внести іншу стадію для цього доручення, дайте запит в РЦ на отримання додаткових прав доступу");
}

void COrdersInsertHelper::adminLogged(const bool admin_logged) {

	this->admin_logged = admin_logged;
}

void COrdersInsertHelper::SetOrderTypeWidget(CDbComboBox *order_type) {

	this->order_type = order_type;
}

void COrdersInsertHelper::SetDefenderWidget(CDbComboBox *defender) {

	this->defender = defender;
}

void COrdersInsertHelper::SetClientWidget(XWidget *client) {

	this->client = client;
}

void COrdersInsertHelper::SetClientBirthDateWidget(XWidget *bdate) {

	this->bdate = bdate;
}

std::shared_ptr<IInsertBinder> COrdersInsertHelper::getIdOrderBinder() {

	return id_order_binder;
}

inline std::shared_ptr<IInsertBinder> COrdersInsertHelper::getOrderDateBinder() {

	return order_date_binder;
}
#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class XComboBox;
class CDbTable;

class COrdersInserter : public CDbInserter {
	enum {
		FIELDS_COUNT = 15
	};
	std::shared_ptr<IDbConnection> conn;
	bool admin_logged;

	CDbComboBox *center;
	XWidget *id_order;
	XWidget *order_date;
	CDbComboBox *order_type;
	CDbComboBox *advocat;
	XWidget *client;
	XWidget *bdate;

	std::shared_ptr<IInsertBinder> center_binder;
	std::shared_ptr<IInsertBinder> id_order_binder;
	std::shared_ptr<IInsertBinder> order_date_binder;

public:
	COrdersInserter();

	COrdersInserter(const COrdersInserter &obj) = delete;
	COrdersInserter(COrdersInserter &&obj) = delete;
	COrdersInserter &operator=(const COrdersInserter &obj) = delete;
	COrdersInserter &operator=(COrdersInserter &&obj) = delete;

	inline void adminLogged(const bool admin_logged);

	void SetCenterBox(CDbComboBox *center);
	void SetIdOrderWidget(XWidget *id_order);
	void SetOrderDateWidget(XWidget *order_date);
	inline void SetOrderTypeWidget(CDbComboBox *order_type);
	inline void SetAdvocatWidget(CDbComboBox *advocat);
	inline void SetClientWidget(XWidget *client);
	inline void SetClientBirthDateWidget(XWidget *bdate);

	inline std::shared_ptr<IInsertBinder> getCenterBinder();
	inline std::shared_ptr<IInsertBinder> getIdOrderBinder();
	inline std::shared_ptr<IInsertBinder> getOrderDateBinder();

	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~COrdersInserter();
};

//*****************************************************

void COrdersInserter::adminLogged(const bool admin_logged) {

	this->admin_logged = admin_logged;
}

void COrdersInserter::SetOrderTypeWidget(CDbComboBox *order_type) {

	this->order_type = order_type;
}

void COrdersInserter::SetAdvocatWidget(CDbComboBox *advocat) {

	this->advocat = advocat;
}

void COrdersInserter::SetClientWidget(XWidget *client) {

	this->client = client;
}

void COrdersInserter::SetClientBirthDateWidget(XWidget *bdate) {

	this->bdate = bdate;
}

std::shared_ptr<IInsertBinder> COrdersInserter::getCenterBinder() {

	return center_binder;
}

std::shared_ptr<IInsertBinder> COrdersInserter::getIdOrderBinder() {

	return id_order_binder;
}

inline std::shared_ptr<IInsertBinder> COrdersInserter::getOrderDateBinder() {

	return order_date_binder;
}
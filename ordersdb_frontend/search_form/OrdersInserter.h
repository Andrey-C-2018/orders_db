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

	CDbComboBox *center;
	XWidget *id_order;
	XWidget *order_date;
	CDbComboBox *order_type;
	CDbComboBox *advocat;
	XWidget *client;
	XWidget *bdate;
	XWidget *reason;
	XWidget *cancel_order;
	XWidget *cancel_date;

public:
	COrdersInserter();

	COrdersInserter(const COrdersInserter &obj) = delete;
	COrdersInserter(COrdersInserter &&obj) = delete;
	COrdersInserter &operator=(const COrdersInserter &obj) = delete;
	COrdersInserter &operator=(COrdersInserter &&obj) = delete;

	inline void SetCenterBox(CDbComboBox *center);
	inline void SetIdOrderWidget(XWidget *id_order);
	inline void SetOrderDateWidget(XWidget *order_date);
	inline void SetOrderTypeWidget(CDbComboBox *order_type);
	inline void SetAdvocatWidget(CDbComboBox *advocat);
	inline void SetClientWidget(XWidget *client);
	inline void SetClientBirthDateWidget(XWidget *bdate);
	inline void SetCancelReasonWidget(XWidget *reason);
	inline void SetCancelOrderWidget(XWidget *cancel_order);
	inline void SetCancelDateWidget(XWidget *cancel_date);

	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~COrdersInserter();
};

//*****************************************************

void COrdersInserter::SetCenterBox(CDbComboBox *center) {

	this->center = center;
}

void COrdersInserter::SetIdOrderWidget(XWidget *id_order) {

	this->id_order = id_order;
}

void COrdersInserter::SetOrderDateWidget(XWidget *order_date) {

	this->order_date = order_date;
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

void COrdersInserter::SetCancelReasonWidget(XWidget *reason) {

	this->reason = reason;
}

void COrdersInserter::SetCancelOrderWidget(XWidget *cancel_order) {

	this->cancel_order = cancel_order;
}

void COrdersInserter::SetCancelDateWidget(XWidget *cancel_date) {

	this->cancel_date = cancel_date;
}
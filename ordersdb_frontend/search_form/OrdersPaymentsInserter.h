#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class XComboBox;
class CDbTable;

class COrdersPaymentsInserter : public CDbInserter {
	enum {
		FIELDS_COUNT = 35
	};
	std::shared_ptr<IDbConnection> conn;

	CDbComboBox *center;
	XWidget *id_order;
	XWidget *order_date;
	CDbComboBox *order_type;
	XWidget *client;
	XWidget *bdate;
	XWidget *reason;
	XWidget *cancel_order;
	XWidget *cancel_date;

public:
	COrdersPaymentsInserter();

	COrdersPaymentsInserter(const COrdersPaymentsInserter &obj) = delete;
	COrdersPaymentsInserter(COrdersPaymentsInserter &&obj) = delete;
	COrdersPaymentsInserter &operator=(const COrdersPaymentsInserter &obj) = delete;
	COrdersPaymentsInserter &operator=(COrdersPaymentsInserter &&obj) = delete;

	void prepare(std::shared_ptr<IDbConnection> conn) override;
	void insert() override;

	virtual ~COrdersPaymentsInserter();
};


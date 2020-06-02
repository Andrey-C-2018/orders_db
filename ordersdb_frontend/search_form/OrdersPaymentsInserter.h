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
	std::shared_ptr<CDbTable> db_table;

public:
	COrdersPaymentsInserter();

	COrdersPaymentsInserter(const COrdersPaymentsInserter &obj) = delete;
	COrdersPaymentsInserter(COrdersPaymentsInserter &&obj) = delete;
	COrdersPaymentsInserter &operator=(const COrdersPaymentsInserter &obj) = delete;
	COrdersPaymentsInserter &operator=(COrdersPaymentsInserter &&obj) = delete;

	virtual ~COrdersPaymentsInserter();
};


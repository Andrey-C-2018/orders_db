#pragma once
#include <db_ext/DbInserter.h>
#include <db_controls/InsertionBinders.h>

class CPaymentsInserter : public CDbInserter {
public:
	CPaymentsInserter();
	virtual ~CPaymentsInserter();
};
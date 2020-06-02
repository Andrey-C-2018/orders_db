#include "OrdersPaymentsInserter.h"

COrdersPaymentsInserter::COrdersPaymentsInserter() : \
					CDbInserter("payments", FIELDS_COUNT) { }

COrdersPaymentsInserter::~COrdersPaymentsInserter() { }

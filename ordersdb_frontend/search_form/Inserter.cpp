#include "Inserter.h"

CInserter::CInserter() { }

void CInserter::prepare(std::shared_ptr<IDbConnection> conn) {

	orders_inserter.prepare(conn);
	payments_inserter.prepare(conn);
}

void CInserter::insert() {

	if (orders_inserter.insert())
		payments_inserter.insert();
}

CInserter::~CInserter() { }

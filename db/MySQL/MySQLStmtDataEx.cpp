#include "MySQLStmtDataEx.h"

MySQLStmtDataEx::~MySQLStmtDataEx() {

	if (params_bindings) {
		assert(params_count > 0);
		delete[] params_bindings;
	}
}
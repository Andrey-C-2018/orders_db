#include "MySQLStmtDataEx.h"

MySQLStmtDataEx::~MySQLStmtDataEx() {

	if (params_bindings) {
		assert(params_count);
		delete[] params_bindings;
	}
}
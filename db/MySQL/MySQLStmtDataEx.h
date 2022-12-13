#pragma once
#include <assert.h>
#include "MySQLStmtData.h"

struct MySQLStmtDataEx final : MySQLStmtData {
private:
	size_t params_count;
	MYSQL_BIND *params_bindings;

public:
	inline MySQLStmtDataEx() noexcept;
	inline explicit MySQLStmtDataEx(MYSQL_STMT *stmt_);

	inline MySQLStmtDataEx(const MySQLStmtDataEx &obj) = delete;
	inline MySQLStmtDataEx(MySQLStmtDataEx &&obj) noexcept;
	inline MySQLStmtDataEx &operator=(const MySQLStmtDataEx &obj) = delete;
	inline MySQLStmtDataEx &operator=(MySQLStmtDataEx &&obj) noexcept;

	void restoreStmt(MYSQL_STMT *stmt_) override;

	inline size_t getParamsCount() const { return params_count; }
	inline const MYSQL_BIND &getBinding(const size_t index) const;
	inline MYSQL_BIND &getBinding(const size_t index);
	inline void applyParamsBindings() const;

	virtual ~MySQLStmtDataEx();
};

//*****************************************************

MySQLStmtDataEx::MySQLStmtDataEx() noexcept : params_count(0), \
											params_bindings(nullptr) { }

MySQLStmtDataEx::MySQLStmtDataEx(MYSQL_STMT *stmt_) : \
											params_count(0), \
											params_bindings(nullptr), \
											MySQLStmtData(stmt_) {

	params_count = mysql_stmt_param_count(stmt);
	if (params_count) {
		params_bindings = new MYSQL_BIND[params_count];
		memset(params_bindings, 0, sizeof(MYSQL_BIND) * params_count);
	}
}

MySQLStmtDataEx::MySQLStmtDataEx(MySQLStmtDataEx &&obj) noexcept : \
										params_count(obj.params_count), \
										params_bindings(obj.params_bindings) {

	obj.params_count = 0;
	obj.params_bindings = nullptr;
}

MySQLStmtDataEx &MySQLStmtDataEx::operator=(MySQLStmtDataEx &&obj) noexcept {

	params_count = obj.params_count;
	params_bindings = obj.params_bindings;

	obj.params_count = 0;
	obj.params_bindings = nullptr;
}

const MYSQL_BIND &MySQLStmtDataEx::getBinding(const size_t index) const {

	assert(index < params_count);
	return params_bindings[index];
}

MYSQL_BIND &MySQLStmtDataEx::getBinding(const size_t index) {

	assert(index < params_count);
	return params_bindings[index];
}

void MySQLStmtDataEx::applyParamsBindings() const {

	if(params_count)
		mysql_stmt_bind_param(stmt, params_bindings);
}
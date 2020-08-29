#pragma once
#include "MySQLVariant.h"

struct MySQLBindingItem final {
	mutable CMySQLVariant value;
	unsigned long length;
	my_bool is_null;
	my_bool error;

	MySQLBindingItem() : value(), length(0), \
						is_null(0), error(0) { }

	MySQLBindingItem(const MySQLBindingItem &obj) = delete;
	MySQLBindingItem(MySQLBindingItem &&obj) = default;
	MySQLBindingItem &operator=(const MySQLBindingItem &obj) = delete;
	MySQLBindingItem &operator=(MySQLBindingItem &&obj) = default;
};
#pragma once
#include <date/Variant.h>

struct IDbStaticResultSet {

	virtual void init(size_t fields_count_, size_t records_count) = 0;
	virtual void setValue(size_t field, size_t record, Variant value) = 0;
	virtual ~IDbStaticResultSet() { }
};

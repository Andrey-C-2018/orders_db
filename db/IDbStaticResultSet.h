#pragma once
#include <date/Variant.h>

struct IDbStaticResultSet {

	virtual void init(size_t fields_count_, size_t records_count) = 0;
	virtual size_t getFieldRecId() const = 0;
	virtual bool gotoRecord(size_t record, int field_id_value) = 0;
	virtual void setValue(size_t field, Variant value) = 0;
	virtual ~IDbStaticResultSet() { }
};

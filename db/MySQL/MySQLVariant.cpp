#include "MySQLVariant.h"

CMySQLVariant::CMySQLVariant() : value_type(MYSQL_TYPE_NULL), value_int(0) { }

CMySQLVariant::CMySQLVariant(const enum_field_types type) : \
								value_type(type), value_int(0) {

	assert(!IsVectorType());
	if (IsDate())
		memset(&value_datetime, 0, sizeof(MYSQL_TIME));
}

CMySQLVariant::CMySQLVariant(const enum_field_types type, const size_t max_len) : \
								value_type(type), value_int(0) {

	if(IsVectorType())
		value_string.reserve(max_len + 1);
	if(IsDate())
		memset(&value_datetime, 0, sizeof(MYSQL_TIME));
}

Variant CMySQLVariant::toStdVariant() const {

	if (IsDate())
		return Variant(value_datetime.day, \
					value_datetime.month, \
					value_datetime.year);

	if (IsVectorType())
		return fromString(Tchar());

	if (IsInteger())
		return Variant(GetInt());

	return Variant();
}

CMySQLVariant::~CMySQLVariant(){ }

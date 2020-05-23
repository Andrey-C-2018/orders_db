#include "MetaInfo.h"

CMetaInfo::CMetaInfo() : invisible_fields_count(0) { }

void CMetaInfo::addInvisibleFieldAsRef(const char *field_name, const char *table_name, \
								const char *ref_field_name, const char *ref_table_name, \
								const bool is_prim_key) {

	size_t ref_field = getFieldIndexByName(ref_field_name, ref_table_name);

	auto db_field_ptr = meta_info.getField(ref_field);
	size_t index = meta_info.addField(field_name, table_name, db_field_ptr);
		
	meta_info.markFieldAsPrimaryKey(index);
	++invisible_fields_count;
}

CMetaInfo::~CMetaInfo() { }

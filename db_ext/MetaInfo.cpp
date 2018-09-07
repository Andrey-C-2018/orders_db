#include "MetaInfo.h"

CMetaInfo::CMetaInfo(){ }

std::shared_ptr<const IDbField> CMetaInfo::getField(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field;
}

const CDbFieldProperties<Tstring> &CMetaInfo::getFieldProperties(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field_props;
}

void CMetaInfo::setPrimaryTable(const Tchar *table_name) {

	assert(table_name);
	auto p_table = std::find(fields.cbegin(), fields.cend(), table_name);
	primary_table_id = std::distance(fields.cbegin(), p_table);
}

int CMetaInfo::addFields(std::shared_ptr<IDbResultSetMetadata> metadata) {
	size_t fields_count = metadata->getFieldsCount();
	CFieldRecord rec;

	updateTableNameIndex();

	for (size_t i = 0; i < fields_count; ++i) {
		rec.field = metadata->getField(i);
		rec.field_props = rec.field->getFieldProperties();
		rec.id = fields.size();
		
		const Tchar *table_name = rec.field_props.table_name.c_str();
		auto p_table_name = FindTableRecord(table_name);

		if (!IsTableRecordFound(p_table_name, table_name)) 
			rec.id_table = addNewTableRecord(table_name);
		else
			rec.id_table = tables[*p_table_name].id;

		rec.order = i;

		fields.emplace_back(rec);
	}
}

CMetaInfo::~CMetaInfo(){ }

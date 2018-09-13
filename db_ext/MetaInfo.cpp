#include <basic/TextConv.h>
#include "MetaInfo.h"

inline void appendAndConvIfNecessary(std::string &str1, const char *str2) { 

	str1 += str2;
}

inline void appendAndConvIfNecessary(std::string &str1, const wchar_t *str2) {
	std::string &str1_ref = str1;

	UCS16_ToDefEnc(str2, -1, str1, \
					[&str1_ref](const char ch) { str1_ref += ch; });
}

inline const char *convIfNecessary(const char *str, std::vector<char> &buffer) {

	return str;
}

inline const char *convIfNecessary(const wchar_t *str, std::vector<char> &buffer) {
	
	UCS16_ToDefEnc(str, -1, buffer, \
					[&buffer](const char ch) {

						buffer.push_back(ch);
					});
	buffer.push_back(0);
	return &buffer[0];
}

inline CDbFieldProperties<std::string> \
	getFieldPropertiesT(std::shared_ptr<IDbField> field, char type_hint){

	return field->getFieldProperties();
}

inline CDbFieldProperties<std::wstring> \
	getFieldPropertiesT(std::shared_ptr<IDbField> field, wchar_t type_hint) {

	return field->getFieldPropertiesW();
}

//*************************************************************

class AddPrimKeyToWhereStmt {
	const CMetaInfo &meta_info;
	std::string &query;

public:
	AddPrimKeyToWhereStmt(const CMetaInfo &meta_info_, std::string &query_) : \
							meta_info(meta_info_), query(query_){ }

	inline void operator()(const CMetaInfo::id_type field_order) {
		auto field_props = meta_info.getFieldProperties(field_order);
		
		appendAndConvIfNecessary(query, field_props.field_name.c_str());
		query += " = ? AND ";
	}
};

//*************************************************************

CMetaInfo::CMetaInfo() : primary_table_id(1){ }

std::shared_ptr<const IDbField> CMetaInfo::getField(const size_t field_order) const {

	assert(field_order < fields.size());
	return fields[fields_index_order[field_order]].field;
}

const CDbFieldProperties<Tstring> &CMetaInfo::getFieldProperties(const size_t field_order) const {

	assert(field_order < fields.size());
	return fields[fields_index_order[field_order]].field_props;
}

void CMetaInfo::setPrimaryTable(const Tchar *table_name) {

	assert(table_name);
	auto p_table = std::find(fields.cbegin(), fields.cend(), table_name);
	primary_table_id = std::distance(fields.cbegin(), p_table);
}

CMetaInfo::id_type CMetaInfo::addTableRecord(const char *table_name) {
	auto p_table_name = findTableRecord(table_name);
	id_type table_id = 0;
	
	if (!isTableRecordFound(p_table_name, table_name)) {
		CTableRecord rec;

		rec.id = table_id = tables.size();
		rec.table_name = table_name;

		tables.emplace_back(rec);

		size_t new_index = tables.size();
		tables_index_name.insert(p_table_name, new_index);
		tables_index_id.insert(findTableRecord(table_id), new_index);
	}
	else
		table_id = tables[*p_table_name].id;

	return table_id;
}

void CMetaInfo::addField(std::shared_ptr<IDbField> field, const int field_order) {
	CFieldRecord rec;
	std::vector<char> conv_buffer;

	auto p_field_order = findFieldRecord(field_order);
	if (isFieldRecordFound(p_field_order, field_order)) {
		XException e(0, _T("this field is already added at position "));

		e << field_order << _T(": '") << fields[*p_field_order].field_props.field_name;
		e << _T("'. The new one ('") << rec.field_props.field_name;
		e << _T("') cannot be added");
		throw e;
	}

	const Tchar *field_name = rec.field_props.field_name.c_str();

	auto p_field = findFieldRecord(field_name);
	if (isFieldRecordFound(p_field, field_name)) {
		XException e(0, _T("the field '"));

		e << field_name << _T("' is already added. Table: '");
		e << rec.field_props.table_name << _T("'");
		throw e;
	}

	rec.id = fields.size();
		
	const Tchar *table_name_t = rec.field_props.table_name.c_str();
	const char *table_name = convIfNecessary(table_name_t, conv_buffer);
	rec.id_table = addTableRecord(table_name);

	rec.order = field_order;
	rec.field = field;
	rec.field_props = getFieldPropertiesT(field, Tchar());
	fields.emplace_back(rec);

	size_t new_index = fields.size();
	fields_index_order.insert(p_field_order, new_index);
	fields_index_name.insert(p_field, new_index);
}

void CMetaInfo::getUpdateQueryForField(const size_t field_order, std::string &query) {
	
	assert(field_order < fields.size());
	CFieldRecord &field_rec = fields[fields_index_order[field_order]];
	id_type updated_table_id = field_rec.id_table;

	auto p_table = findTableRecord(updated_table_id);
	assert(isTableRecordFound(p_table, updated_table_id));

	query.clear();
	query = "UPDATE ";
	query += tables[*p_table].table_name;
	
	query += " SET ";
	const Tchar *field_name = field_rec.field_props.field_name.c_str();
	appendAndConvIfNecessary(query, field_name);
	query += " = ? ";

	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

void CMetaInfo::getDeleteQuery(std::string &query) {

	query.clear();
	query = "DELETE FROM ";
	query += primary_table_name;
	
	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

CMetaInfo::~CMetaInfo(){ }

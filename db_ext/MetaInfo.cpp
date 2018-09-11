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

CMetaInfo::CMetaInfo() : predTableName(tables), predTableId(tables), \
							predKeyTableId(keys), primary_table_id(1){ }

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

void CMetaInfo::addField(std::shared_ptr<IDbField> field, const int order) {
	CFieldRecord rec;

	rec.field = field;
	rec.field_props = getFieldPropertiesT(field, Tchar());
	rec.id = fields.size();
		
	const Tchar *table_name_t = rec.field_props.table_name.c_str();
	std::vector<char> table_name;
	UCS16_ToDefEnc(table_name_t, -1, table_name, \
		[&table_name](const char ch) {
		table_name.push_back(ch);
	});
	table_name.push_back(0);

	auto p_table_name = findTableRecord(&table_name[0]);

	if (!isTableRecordFound(p_table_name, &table_name[0])) 
		rec.id_table = addNewTableRecord(&table_name[0]);
	else
		rec.id_table = tables[*p_table_name].id;

	rec.order = order;
	fields.emplace_back(rec); //better get an insert pos by std::lower_bound
}

void CMetaInfo::getUpdateStr(const size_t field_order, std::string &query) {
	
	assert(field_order < fields.size());
	id_type updated_table_id = fields[field_order].id_table;

	auto p_table = findTableRecord(updated_table_id);
	assert(isTableRecordFound(p_table, updated_table_id));

	query.clear();
	query = "UPDATE ";
	query += tables[*p_table].table_name;
	
	query += " SET ";
	const Tchar *field_name = fields[field_order].field_props.field_name.c_str();
	appendAndConvIfNecessary(query, field_name);
	query += " = ? ";

	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

void CMetaInfo::getDeleteStr(std::string &query) {

	query.clear();
	query = "DELETE FROM ";
	query += primary_table_name;
	
	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

CMetaInfo::~CMetaInfo(){ }

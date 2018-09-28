#include <basic/TextConv.h>
#include "MetaInfo.h"

class AddPrimKeyToWhereStmt {
	const CMetaInfo &meta_info;
	std::string &query;

public:
	AddPrimKeyToWhereStmt(const CMetaInfo &meta_info_, std::string &query_) : \
							meta_info(meta_info_), query(query_){ }

	inline void operator()(const size_t field) {
		
		query += " = ? AND ";
		query += meta_info.getFieldName(field).str;
	}
};

//*************************************************************

CMetaInfo::CMetaInfo() : primary_table_id(-1){ }

void CMetaInfo::setPrimaryTable(const char *table_name) {

	assert(table_name);

	auto p_table = std::find_if(tables.cbegin(), tables.cend(), \
		[table_name](const CTableRecord &rec) {

			return !strcmp(rec.table_name.str, table_name);
		});

	if (p_table == tables.cend()) {
		XException e(0, _T("the query does not refer to such table: "));
		e << table_name;
		throw e;
	}

	primary_table_name = p_table->table_name.str;
	primary_table_id = p_table->id;
}

CMetaInfo::id_type CMetaInfo::addTableRecord(ImmutableString<char> table_name) {
	auto p_table_name = findTableRecord(table_name.str);
	id_type table_id = 0;
	
	if (!isTableRecordFound(p_table_name, table_name.str)) {
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

void CMetaInfo::addField(std::shared_ptr<IDbField> field, const size_t new_field_index) {
	CFieldRecord rec;
	ImmutableString<char> new_field_name = field->getFieldName();

	assert(new_field_index <= fields.size());

	ConstIndexIterator p_field = findFieldRecord(new_field_name.str);
	if (isFieldRecordFound(p_field, new_field_name.str)) {
		XException e(0, _T("the field '"));

		e << fields[*p_field].field_name.str << _T("' is already added. Table: '");
		e << field->getTableName().str << _T("'");
		throw e;
	}

	rec.id = fields.size();
	rec.field = field;
	rec.field_name = new_field_name;
			
	ImmutableString<char> table_name = field->getTableName();
	rec.id_table = addTableRecord(table_name);

	rec.is_primary_key = field->isPrimaryKey();
	fields.insert(fields.begin() + new_field_index, rec);

	if (new_field_index == fields.size() - 1) {
		fields_index_id.insert(p_field, fields.size());
		fields_index_name.insert(p_field, fields.size());
	}
	else {
		typedef CIndexedValueSearchPredicate<id_type, IndexType, \
												std::vector<CFieldRecord>, \
												CGetFieldIdByIndex> PredId;
		typedef CIndexedTextSearchPredicate<Tchar, IndexType, \
												std::vector<CFieldRecord>, \
												CGetFieldNameByIndex> PredName;
		PredId predFieldId(fields);
		PredName predFieldName(fields);

		std::sort(fields_index_id.begin(), fields_index_id.end(), predFieldId);
		std::sort(fields_index_name.begin(), fields_index_name.end(), predFieldName);
	}

	if (rec.is_primary_key) {
		auto p_keys = findTableKeys(rec.id_table);
		auto p_new_key = p_keys.second == keys_index_table.cend() ? \
						keys_index_table.cend() : p_keys.second + 1;

		keys_index_table.insert(p_new_key, new_field_index);
	}
}

void CMetaInfo::getUpdateQueryForField(const size_t field, std::string &query) {
	
	assert(field < fields.size());
	assert(primary_table_id != -1);
	CFieldRecord &field_rec = fields[field];
	id_type updated_table_id = field_rec.id_table;

	auto p_table = findTableRecord(updated_table_id);
	assert(isTableRecordFound(p_table, updated_table_id));

	query.clear();
	query = "UPDATE ";
	query += tables[*p_table].table_name.str;
	
	query += " SET ";
	query += fields[field].field_name.str;
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

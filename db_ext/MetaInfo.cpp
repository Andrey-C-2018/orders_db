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

class BindPrimKeyValue {
	const CMetaInfo &meta_info;
	size_t curr_key_no;
	std::shared_ptr<const IDbResultSet> prim_key_values_src;
	std::shared_ptr<IDbBindingTarget> binding_target;
	
public:
	BindPrimKeyValue(const CMetaInfo &meta_info_, \
					const size_t initial_key_no, \
					std::shared_ptr<const IDbResultSet> prim_key_values_src_, \
					std::shared_ptr<IDbBindingTarget> binding_target_) : \
						meta_info(meta_info_), curr_key_no(initial_key_no), \
						prim_key_values_src(prim_key_values_src_), \
						binding_target(binding_target_) { }

	inline void operator()(const size_t field) {

		meta_info.getField(field)->getValueAndBindItTo(prim_key_values_src, \
														binding_target, curr_key_no);
		++curr_key_no;
	}
};

//*************************************************************

CMetaInfo::CMetaInfo() : primary_table_id(-1){

	fields.reserve(DEF_FIELDS_COUNT);
	tables.reserve(DEF_TABLES_COUNT);
	keys_index_table.reserve(DEF_TABLE_KEY_SIZE);
}

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

void CMetaInfo::addKeyIndex(const id_type id_table, const id_type id_field) {
	
	auto p_keys = findTableKeys(id_table);
	auto p_new_key = p_keys.second == keys_index_table.cend() ? \
						keys_index_table.cend() : p_keys.second + 1;

	keys_index_table.insert(p_new_key, id_field);
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
	rec.field_size = field->getFieldMaxLength();
			
	ImmutableString<char> table_name = field->getTableName();
	rec.id_table = addTableRecord(table_name);

	rec.is_primary_key = field->isPrimaryKey();
	fields.insert(fields.begin() + new_field_index, rec);

	if (new_field_index == fields.size() - 1) {
		fields_index_id.insert(p_field, fields.size());
		fields_index_name.insert(p_field, fields.size());
	}
	else
		refreshFieldIndexes();

	if (rec.is_primary_key) 
		addKeyIndex(rec.id_table, new_field_index);
}

void CMetaInfo::refreshFieldIndexes() {
	
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

void CMetaInfo::clearAndAddFields(std::shared_ptr<IDbResultSetMetadata> fields) {
	size_t fields_count = fields->getFieldsCount();

	this->fields.clear();
	this->tables.clear();
	this->fields.reserve(fields_count);

	for (size_t i = 0; i < fields_count; ++i) {
		CFieldRecord rec;
		rec.field = fields->getField(i);
		ImmutableString<char> table_name = rec.field->getTableName();

		rec.id = i;
		rec.id_table = addTableRecord(table_name);
		rec.field_name = rec.field->getFieldName();
		rec.field_size = rec.field->getFieldMaxLength();
		rec.is_primary_key = rec.field->isPrimaryKey();
	
		if (rec.is_primary_key)
			addKeyIndex(rec.id_table, i);

		this->fields.emplace_back(rec);
	}

	refreshFieldIndexes();
}

void CMetaInfo::getUpdateQueryForField(const size_t field, std::string &query) const {
	
	assert(field < fields.size());
	const CFieldRecord &field_rec = fields[field];
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
	enumeratePrimKey(tables[*p_table].id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

void CMetaInfo::getDeleteQuery(std::string &query) const {

	assert(primary_table_id != -1);
	query.clear();
	query = "DELETE FROM ";
	query += primary_table_name;
	
	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query));

	size_t count = query.size() - sizeof("AND ") / sizeof(char);
	query.erase(query.size() - count, count);
}

void CMetaInfo::bindPrimaryKeyValues(const size_t field, \
									std::shared_ptr<const IDbResultSet> prim_key_values_src, \
									std::shared_ptr<IDbBindingTarget> binding_target) const {

	assert(field < fields.size());
	const CFieldRecord &field_rec = fields[field];
	id_type updated_table_id = field_rec.id_table;

	auto p_table = findTableRecord(updated_table_id);
	assert(isTableRecordFound(p_table, updated_table_id));

	enumeratePrimKey(tables[*p_table].id, BindPrimKeyValue(*this, 1, prim_key_values_src, \
															binding_target));
}

void CMetaInfo::bindPrimaryKeyValues(std::shared_ptr<const IDbResultSet> prim_key_values_src, \
										std::shared_ptr<IDbBindingTarget> binding_target) const {
	
	assert(primary_table_id != -1);
	enumeratePrimKey(primary_table_id, BindPrimKeyValue(*this, 0, prim_key_values_src, \
															binding_target));
}

CMetaInfo::~CMetaInfo(){ }

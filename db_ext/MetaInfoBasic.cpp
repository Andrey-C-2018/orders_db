#include <basic/TextConv.h>
#include "MetaInfoBasic.h"

CMetaInfoBasicException::CMetaInfoBasicException(const int err_code, \
											const Tchar *err_descr) : \
											XException(err_code, err_descr) { }

CMetaInfoBasicException::CMetaInfoBasicException(const CMetaInfoBasicException &obj) : \
											XException(obj) { }

CMetaInfoBasicException::~CMetaInfoBasicException() { }

//*************************************************************

class AddPrimKeyToWhereStmt {
	const CMetaInfoBasic &meta_info;
	std::string &query;
	const char *table_alias;

public:
	AddPrimKeyToWhereStmt(const CMetaInfoBasic &meta_info_, std::string &query_, \
							const char *table_alias_) : \
							meta_info(meta_info_), query(query_), \
							table_alias(table_alias_){ }

	inline void operator()(const size_t field) {
		
		if (table_alias) {
			query += table_alias;
			query += '.';
		}
		query += meta_info.getFieldName(field).str;
		query += " = ? AND ";
	}
};

class BindPrimKeyValue {
	const CMetaInfoBasic &meta_info;
	size_t curr_key_no;
	std::shared_ptr<const IDbResultSet> prim_key_values_src;
	std::shared_ptr<IDbBindingTarget> binding_target;
	
public:
	BindPrimKeyValue(const CMetaInfoBasic &meta_info_, \
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

std::string CMetaInfoBasic::constant_modifier;

CMetaInfoBasic::CMetaInfoBasic() : primary_table_id(-1){

	fields.reserve(DEF_FIELDS_COUNT);
	tables.reserve(DEF_TABLES_COUNT);
	
	fields_index_name.reserve(DEF_FIELDS_COUNT);
	
	tables_index_id.reserve(DEF_TABLES_COUNT);
	tables_index_name.reserve(DEF_TABLES_COUNT);
	
	keys_index_table.reserve(DEF_TABLE_KEY_SIZE);
}

void CMetaInfoBasic::setPrimaryTable(const char *table_name) {

	assert(table_name);

	auto p_table = std::find_if(tables.cbegin(), tables.cend(), \
		[table_name](const CTableRecord &rec) {

			return rec.table_name == table_name;
		});

	if (p_table == tables.cend()) {
		CMetaInfoBasicException e(CMetaInfoBasicException::E_TABLE, \
							_T("the query does not refer to such table: "));
		e << table_name;
		throw e;
	}

	primary_table_name = p_table->table_name;
	primary_table_id = p_table->id;
}

void CMetaInfoBasic::setQueryConstantModifier(ImmutableString<char> modifier) {

	constant_modifier = modifier.str;
}

CMetaInfoBasic::id_type CMetaInfoBasic::addTableRecord(std::string &table_name, \
														ConstIndexIterator p_table_name) {

	CTableRecord rec;
	size_t new_index = tables.size();

	rec.id = (id_type)new_index;
	rec.table_name = std::move(table_name);

	tables.emplace_back(rec);
		
	tables_index_name.insert(p_table_name, new_index);
	tables_index_id.push_back(new_index);

	return rec.id;
}

void CMetaInfoBasic::addKeyIndex(const id_type id_table, const IndexType field_index) {
	
	auto p_last_key = findTableLastKey(id_table);
	auto p_new_key = p_last_key == keys_index_table.cend() ? \
						keys_index_table.cend() : p_last_key + 1;

	keys_index_table.insert(p_new_key, field_index);
}

size_t CMetaInfoBasic::addField(const char *new_field_name, const char *new_table_name, \
								std::shared_ptr<IDbField> field) {
	
	auto p_table_name = findTableRecord(new_table_name);
	id_type new_table_id = -1;
	ConstIndexIterator p_field_nm;
	
	if (isTableRecordFound(p_table_name, new_table_name)) {

		new_table_id = tables[*p_table_name].id;
		p_field_nm = findUniqueFieldRecord(new_field_name, new_table_id);
		if (isUniqueFieldRecordFound(p_field_nm, new_field_name, new_table_id)) {

			CMetaInfoBasicException e(CMetaInfoBasicException::E_FIELD_EXISTS, \
										_T("the field '"));

			e << fields[*p_field_nm].field_name << _T("' is already added. Table: '");
			e << field->getTableName() << _T("'");
			throw e;
		}
	}
	else {
		std::string table_name = new_table_name;
		new_table_id = addTableRecord(table_name, p_table_name);
		p_field_nm = fields_index_name.cend();
	}
	
	size_t new_field_index = fields.size();
	CFieldRecord rec;
	rec.id = (id_type)new_field_index;
	rec.field = field;
	rec.field_name = new_field_name;
	rec.field_size = field->getFieldMaxLength();
	rec.id_table = new_table_id;

	rec.is_primary_key = field->isPrimaryKey();
	fields.emplace_back(rec);
	fields_index_name.insert(p_field_nm, new_field_index);

	if (rec.is_primary_key) 
		addKeyIndex(rec.id_table, new_field_index);

	return new_field_index;
}

void CMetaInfoBasic::markFieldAsPrimaryKey(const size_t field) {

	assert(field < fields.size());
	CFieldRecord &field_rec = fields[field];

	if (!field_rec.is_primary_key) {
		field_rec.is_primary_key = true;
		addKeyIndex(field_rec.id_table, field);
	}
}

void CMetaInfoBasic::refreshFieldIndexes() {
	
	typedef CIndexedValueSearchPredicate<FieldTable, IndexType, std::vector<CFieldRecord>, \
										CGetUniqueFieldNameByIndex> Pred;
	Pred predFieldUniqueName(fields);
	std::sort(fields_index_name.begin(), fields_index_name.end(), predFieldUniqueName);
}

void CMetaInfoBasic::clearAndAddFields(std::shared_ptr<const IDbResultSetMetadata> fields) {
	size_t fields_count = fields->getFieldsCount();

	clear();

	this->fields.reserve(fields_count);
	this->fields_index_name.reserve(fields_count);
	std::string table_name;

	for (size_t i = 0; i < fields_count; ++i) {
		CFieldRecord rec;
		rec.field = fields->getField(i);
		table_name = rec.field->getTableName();
		auto p_table = findTableRecord(table_name.c_str());
		
		rec.id = (id_type)i;
		rec.id_table = isTableRecordFound(p_table, table_name.c_str()) ? \
						tables[*p_table].id : addTableRecord(table_name, p_table);
		rec.field_name = rec.field->getFieldName();
		rec.field_size = rec.field->getFieldMaxLength();
		rec.is_primary_key = rec.field->isPrimaryKey();
	
		if (rec.is_primary_key)
			addKeyIndex(rec.id_table, i);

		this->fields.emplace_back(rec);
		fields_index_name.push_back(i);
	}

	refreshFieldIndexes();
}

size_t CMetaInfoBasic::appendWherePartOfUpdateQuery(std::string &query, \
												const char *table_alias) const {

	assert(primary_table_id != -1);
	size_t prim_key_size = enumeratePrimKey(primary_table_id, \
										AddPrimKeyToWhereStmt(*this, query, table_alias));
	removeEmptyAndStmt(query);

	return prim_key_size;
}

size_t CMetaInfoBasic::appendWherePartOfUpdateQuery(const char *table_name, std::string &query, \
												const char *table_alias) const {

	auto p_table = findTableRecord(table_name);
	assert(isTableRecordFound(p_table, table_name));

	size_t table_key_size = enumeratePrimKey(tables[*p_table].id, \
										AddPrimKeyToWhereStmt(*this, query, table_alias));
	removeEmptyAndStmt(query);

	return table_key_size;
}

void CMetaInfoBasic::getUpdateQueryForField(const size_t field, std::string &query) const {
	
	assert(field < fields.size());
	const CFieldRecord &field_rec = fields[field];
	id_type updated_table_id = field_rec.id_table;

	auto p_table = findTableRecord(updated_table_id);
	assert(isTableRecordFound(p_table, updated_table_id));

	query.clear();
	query = "UPDATE ";
	query += tables[*p_table].table_name;
	
	query += " SET ";
	if (!constant_modifier.empty()) {
		query += constant_modifier;
		query += ',';
	}
	query += fields[field].field_name;
	query += " = ? WHERE ";

	enumeratePrimKey(tables[*p_table].id, AddPrimKeyToWhereStmt(*this, query, nullptr));

	removeEmptyAndStmt(query);
}

void CMetaInfoBasic::getDeleteQuery(std::string &query) const {

	assert(primary_table_id != -1);
	query.clear();
	query = "DELETE FROM ";
	query += primary_table_name;
	
	query += " WHERE ";
	enumeratePrimKey(primary_table_id, AddPrimKeyToWhereStmt(*this, query, nullptr));

	removeEmptyAndStmt(query);
}

void CMetaInfoBasic::bindPrimaryKeyValues(\
									std::shared_ptr<const IDbResultSet> prim_key_values_src, \
									std::shared_ptr<IDbBindingTarget> binding_target) const {

	enumeratePrimKey(primary_table_id, BindPrimKeyValue(*this, 0, \
														prim_key_values_src, binding_target));
}

void CMetaInfoBasic::bindPrimaryKeyValuesWithOffset(const size_t field, \
									const size_t params_offset, \
									std::shared_ptr<const IDbResultSet> prim_key_values_src, \
									std::shared_ptr<IDbBindingTarget> binding_target) const {
	
	assert(field < fields.size());
	const CFieldRecord &field_rec = fields[field];

	enumeratePrimKey(field_rec.id_table, BindPrimKeyValue(*this, params_offset, \
														prim_key_values_src, binding_target));
}

CMetaInfoBasic::~CMetaInfoBasic(){ }

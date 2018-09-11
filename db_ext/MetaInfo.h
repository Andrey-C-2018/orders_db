#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/tstring.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>

class CMetaInfo{
public:
	typedef int id_type;
private:
	typedef size_t IndexType;
	
	typedef std::vector<IndexType>::iterator IndexIterator;
	typedef std::vector<IndexType>::const_iterator ConstIndexIterator;

	struct CFieldRecord {
		id_type id;
		std::shared_ptr<IDbField> field;
		CDbFieldProperties<Tstring> field_props;
		id_type id_table;
		size_t order;

		CFieldRecord() : id(0), id_table(0), order(0) { }
		CFieldRecord(const CFieldRecord &obj) = default;
		CFieldRecord(CFieldRecord &&obj) = default;
		inline bool operator==(const CFieldRecord &obj) const;
		inline bool operator<(const CFieldRecord &obj) const;
	};

	struct CKeyRecord {
		id_type id_field;
		id_type id_table;
		inline bool operator==(const CKeyRecord &obj) const;
		inline bool operator<(const CKeyRecord &obj) const;
	};

	struct CTableRecord {
		id_type id;
		std::string table_name;
		inline bool operator==(const CTableRecord &obj) const;
		inline bool operator<(const CTableRecord &obj) const;
	};

	std::vector<CFieldRecord> fields;
	std::vector<CKeyRecord> keys;
	std::vector<CTableRecord> tables;

	std::vector<IndexType> fields_index_id, fields_index_order;
	std::vector<IndexType> keys_index_table;
	std::vector<IndexType> tables_index_id, tables_index_name;

	class CTablesIndexNamePredicate {
		const std::vector<CTableRecord> &tables;
	public:
		CTablesIndexNamePredicate(const std::vector<CTableRecord> &value);
		inline bool operator()(IndexType l, IndexType r) const;
		inline bool operator()(IndexType l, const std::string &r) const;
		inline bool operator()(IndexType l, const char *r) const;
		inline bool operator()(const std::string &l, IndexType r) const;
		inline bool operator()(const char *l, IndexType r) const;
	} predTableName;

	class CTablesIndexIdPredicate {
		const std::vector<CTableRecord> &tables;
	public:
		CTablesIndexIdPredicate(const std::vector<CTableRecord> &value);
		inline bool operator()(IndexType l, IndexType r) const;
		inline bool operator()(IndexType l, id_type r) const;
		inline bool operator()(id_type l, IndexType r) const;
	} predTableId;

	class CKeysIndexTableIdPredicate {
		const std::vector<CKeyRecord> &keys;
	public:
		CKeysIndexTableIdPredicate(const std::vector<CKeyRecord> &value);
		inline bool operator()(IndexType l, IndexType r) const;
		inline bool operator()(IndexType l, id_type r) const;
		inline bool operator()(id_type l, IndexType r) const;
	} predKeyTableId;

	id_type primary_table_id;
	std::string primary_table_name;

	inline IndexIterator findTableRecord(const char *table_name);
	inline IndexIterator findTableRecord(const id_type id_table);
	inline std::pair<IndexIterator, IndexIterator> findTableKeysRecords(const id_type id_table);

	inline bool isTableRecordFound(const ConstIndexIterator p_table_name, \
									const char *table_name) const;
	inline bool isTableRecordFound(const ConstIndexIterator p_table_id, \
									const id_type id_table) const;

	id_type addNewTableRecord(const char *table_name);

	template <class FieldPredicate> \
		void enumeratePrimKey(const id_type table_id, \
								FieldPredicate field_pred) const;
public:
	CMetaInfo();

	CMetaInfo(const CMetaInfo &obj) = default;
	CMetaInfo(CMetaInfo &&obj) = default;	
	CMetaInfo &operator=(const CMetaInfo &obj) = default;
	CMetaInfo &operator=(CMetaInfo &&obj) = default;
	
	inline size_t getFieldsCount() const;
	inline bool empty() const;
	std::shared_ptr<const IDbField> getField(const size_t field) const;
	const CDbFieldProperties<Tstring> &getFieldProperties(const size_t field) const;

	void setPrimaryTable(const Tchar *table_name);

	void addField(std::shared_ptr<IDbField> field, const int order);

	void getUpdateStr(const size_t field_order, std::string &query);
	void getDeleteStr(std::string &query);

	virtual ~CMetaInfo();
};

//*************************************************************

template <class FieldPredicate> \
void CMetaInfo::enumeratePrimKey(const id_type table_id, \
									FieldPredicate field_pred) const {
	auto table_prim_keys = findTableKeysRecords(table_id);

	size_t counter = 0;
	while (table_prim_keys.first != table_prim_keys.second) {
		id_type id = keys[*(table_prim_keys.first)].id_field;

		auto p_id_key = findFieldRecord(id);
		assert(isFieldRecordFound(p_id_key, id));

		auto rec = &fields[*p_id_key];
		field_pred(rec.order);

		++table_prim_keys.first;
		++counter;
	}
	if (!counter)
		throw CMetaInfoException(CMetaInfoException::E_NO_FKEY, \
			_T("There are no foreign keys for the table the id field belongs to"));
}

bool CMetaInfo::CFieldRecord::operator==(const CMetaInfo::CFieldRecord &obj) const {

	return this->id == obj.id;
}

bool CMetaInfo::CFieldRecord::operator<(const CMetaInfo::CFieldRecord &obj) const {

	return this->id < obj.id;
}

bool CMetaInfo::CKeyRecord::operator==(const CMetaInfo::CKeyRecord &obj) const {

	return this->id_field == obj.id_field;
}

bool CMetaInfo::CKeyRecord::operator<(const CMetaInfo::CKeyRecord &obj) const {

	return this->id_field < obj.id_field;
}

bool CMetaInfo::CTableRecord::operator==(const CMetaInfo::CTableRecord &obj) const {

	return this->id == obj.id;
}

bool CMetaInfo::CTableRecord::operator<(const CMetaInfo::CTableRecord &obj) const {

	return this->id < obj.id;
}

CMetaInfo::IndexIterator CMetaInfo::findTableRecord(const char *table_name) {

	return std::lower_bound(tables_index_name.begin(), \
							tables_index_name.end(), \
							table_name, predTableName);
}

CMetaInfo::IndexIterator CMetaInfo::findTableRecord(const id_type id_table) {

	return std::lower_bound(tables_index_id.begin(), \
							tables_index_id.end(), \
							id_table, predTableId);
}

std::pair<CMetaInfo::IndexIterator, CMetaInfo::IndexIterator> \
	CMetaInfo::findTableKeysRecords(const id_type id_table) {

	return std::equal_range(keys_index_table.begin(), \
							keys_index_table.end(), \
							id_table, predKeyTableId);
}

bool CMetaInfo::isTableRecordFound(const CMetaInfo::ConstIndexIterator p_table_name, \
									const char *table_name) const {

	return p_table_name != tables_index_name.end() && \
		!tables[*p_table_name].table_name.compare(table_name);
}

bool CMetaInfo::isTableRecordFound(const CMetaInfo::ConstIndexIterator p_table_id, \
									const id_type id_table) const {

	return p_table_id != tables_index_id.end() && \
		(id_table == tables[*p_table_id].id);
}

size_t CMetaInfo::getFieldsCount() const {

	return fields.size();
}

bool CMetaInfo::empty() const {

	return fields.empty();
}

#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/tstring.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>

class CMetaInfo{
	typedef size_t IndexType;
	typedef int id_type;

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
		Tstring table_name;
		inline bool operator==(const CTableRecord &obj) const;
		inline bool operator<(const CTableRecord &obj) const;
	};

	std::vector<CFieldRecord> fields;
	std::vector<CKeyRecord> keys;
	std::vector<CTableRecord> tables;

	std::vector<IndexType> fields_index_id, fields_index_order;
	std::vector<IndexType> keys_index_table;
	std::vector<IndexType> tables_index_id, tables_index_name;

	id_type primary_table_id;

	inline IndexIterator FindTableRecord(const Tchar *table_name);
	inline IndexIterator FindTableRecord(const id_type id_table);
	inline ConstIndexIterator FindFieldRecord(const id_type id_field) const;
	inline ConstIndexIterator FindFieldRecord(const size_t field_order_no) const;
	inline std::pair<IndexIterator, IndexIterator> FindTableKeysRecords(const id_type id_table);

	inline bool IsTableRecordFound(const ConstIndexIterator p_table_name, \
									const Tchar *table_name) const;
	inline bool IsTableRecordFound(const ConstIndexIterator p_table_id, \
									const id_type id_table) const;
	inline bool IsFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const;

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

	void addFields(std::shared_ptr<IDbResultSetMetadata> metadata);
	void addField(std::shared_ptr<IDbField> field);

	void getUpdateStr(const size_t updated_field, Tstring &str);
	void getDeleteStr(std::string &str);

	template <class FieldPredicate> \
		void enumeratePrimKey(const size_t updated_field, \
								FieldPredicate field_pred);

	virtual ~CMetaInfo();
};

//*************************************************************

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

CMetaInfo::IndexIterator CMetaInfo::FindTableRecord(const Tchar *table_name) {

	return std::lower_bound(tables_index_name.begin(), \
							tables_index_name.end(), \
							table_name, predTableName);
}

CMetaInfo::IndexIterator CMetaInfo::FindTableRecord(const id_type id_table) {

	return std::lower_bound(tables_index_id.begin(), \
							tables_index_id.end(), \
							id_table, predTableId);
}

CMetaInfo::ConstIndexIterator CMetaInfo::FindFieldRecord(const id_type id_field) const {

	return std::lower_bound(fields_index_id.begin(), \
							fields_index_id.end(), \
							id_field, \
							predFieldId);
}

std::pair<CMetaInfo::IndexIterator, CMetaInfo::IndexIterator> \
	CMetaInfo::FindTableKeysRecords(const id_type id_table) {

	return std::equal_range(keys_index_table.begin(), \
							keys_index_table.end(), \
							id_table, predFKeyTableId);
}

bool CMetaInfo::IsTableRecordFound(const CMetaInfo::ConstIndexIterator p_table_name, \
									const Tchar *table_name) const {

	return p_table_name != tables_index_name.end() && \
		!tables[*p_table_name].table_name.compare(table_name);
}

bool CMetaInfo::IsTableRecordFound(const CMetaInfo::ConstIndexIterator p_table_id, \
									const id_type id_table) const {

	return p_table_id != tables_index_id.end() && \
		(id_table == tables[*p_table_id].id);
}

bool CMetaInfo::IsFieldRecordFound(const CMetaInfo::ConstIndexIterator p_field_id, \
									const id_type id_field) const {

	return p_field_id != fields_index_id.end() && \
		(id_field == fields[*p_field_id].id);
}

size_t CMetaInfo::getFieldsCount() const {

	return fields.size();
}

bool CMetaInfo::empty() const {

	return fields.empty();
}

void CMetaInfo::getUpdateStr(const size_t updated_field, Tstring &str) {

}

void CMetaInfo::getDeleteStr(std::string &str) {

	str.clear();
	str = "DELETE FROM ";
}

template <class FieldPredicate> \
void CMetaInfo::enumeratePrimKey(const size_t updated_field, FieldPredicate field_pred) {
	auto p_field_index = FindFieldRecord(updated_field);
	int table_id = fields[*p_field_index].id_table;
	auto table_prim_keys = FindTableKeysRecords(table_id);

	size_t counter = 0;
	while (table_prim_keys.first != table_prim_keys.second) {
		id_type id = keys[*(table_prim_keys.first)].id_field;

		auto p_id_key = FindFieldRecord(id);
		assert(IsFieldRecordFound(p_id_key, id));

		auto rec = &fields[*p_id_key];
		field_pred(rec.order);

		++table_prim_keys.first;
		++counter;
	}
	if (!counter)
		throw CMetaInfoException(CMetaInfoException::E_NO_FKEY, \
			_T("There are no foreign keys for the table the id field belongs to"));
}
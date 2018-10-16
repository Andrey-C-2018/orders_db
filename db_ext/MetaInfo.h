#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/Exception.h>
#include <basic/NumericTypeWrapper.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>
#include "IndexedSearchPredicates.h"

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
		ImmutableString<char> field_name;
		mutable ImmutableString<wchar_t> field_name_w;
		size_t field_size;
		bool is_primary_key;
		id_type id_table;

		CFieldRecord() : id(0), field_name(nullptr, 0), \
								field_name_w(nullptr, 0), \
								is_primary_key(true), id_table(-1){ }
		CFieldRecord(const CFieldRecord &obj) = default;
		CFieldRecord(CFieldRecord &&obj) = default;

		CFieldRecord &operator=(const CFieldRecord &obj) = default;
		CFieldRecord &operator=(CFieldRecord &&obj) = default;
		inline bool operator==(const CFieldRecord &obj) const;
		inline bool operator<(const CFieldRecord &obj) const;
	};

	struct CTableRecord {
		id_type id;
		ImmutableString<char> table_name;
		mutable ImmutableString<wchar_t> table_name_w;

		CTableRecord() : id(-1), table_name(nullptr, 0), \
								table_name_w(nullptr, 0) { }
		CTableRecord(const CTableRecord &obj) = default;
		CTableRecord(CTableRecord &&obj) = default;

		CTableRecord &operator=(const CTableRecord &obj) = default;
		CTableRecord &operator=(CTableRecord &&obj) = default;
		inline bool operator==(const CTableRecord &obj) const;
		inline bool operator<(const CTableRecord &obj) const;
	};

	std::vector<CFieldRecord> fields;
	std::vector<CTableRecord> tables;

	std::vector<IndexType> fields_index_id, fields_index_name;
	std::vector<IndexType> keys_index_table;
	std::vector<IndexType> tables_index_id, tables_index_name;

	struct CGetFieldNameByIndex {
		inline const char *operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const { return fields[index].field_name.str; }
	};
	struct CGetFieldIdByIndex {
		inline id_type operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const {	return fields[index].id; }
	};
	struct CGetTableNameByIndex {
		inline const char *operator()(const std::vector<CTableRecord> &tables, \
						const size_t index) const {	return tables[index].table_name.str; }
	};
	struct CGetTableIdByIndex {
		inline id_type operator()(const std::vector<CTableRecord> &tables, \
						const size_t index) const {	return tables[index].id; }
	};
	struct CGetTableIdFromKeysByIndex {
		inline id_type operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const {	return fields[index].id_table; }
	};

	id_type primary_table_id;
	std::string primary_table_name;

	inline ConstIndexIterator findFieldRecord(const char *field_name) const;
	inline ConstIndexIterator findFieldRecord(const id_type id_field) const;
	inline ConstIndexIterator findTableRecord(const char *table_name) const;
	inline ConstIndexIterator findTableRecord(const id_type id_table) const;
	inline std::pair<ConstIndexIterator, ConstIndexIterator> \
							findTableKeys(const id_type id_table) const;

	inline bool isFieldRecordFound(const ConstIndexIterator p_field_name, \
									const char *field_name) const;
	inline bool isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const;
	inline bool isTableRecordFound(const ConstIndexIterator p_table_name, \
									const char *table_name) const;
	inline bool isTableRecordFound(const ConstIndexIterator p_table_id, \
									const id_type id_table) const;

	id_type addTableRecord(ImmutableString<char> table_name);

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
	inline ImmutableString<char> getFieldName(const size_t field) const;
	inline ImmutableString<wchar_t> getFieldNameW(const size_t field) const;
	inline ImmutableString<char> getTableName(const size_t field) const;
	inline ImmutableString<wchar_t> getTableNameW(const size_t field) const;
	inline size_t getFieldSize(const size_t field) const;
	inline bool isPrimaryKey(const size_t field) const;

	void setPrimaryTable(const char *table_name);

	void addField(std::shared_ptr<IDbField> field, const size_t new_field_index);

	void getUpdateQueryForField(const size_t field, std::string &query);
	void getDeleteQuery(std::string &query);

	virtual ~CMetaInfo();
};

//*************************************************************

std::shared_ptr<const IDbField> CMetaInfo::getField(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field;
}

ImmutableString<char> CMetaInfo::getFieldName(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field_name;
}

ImmutableString<wchar_t> CMetaInfo::getFieldNameW(const size_t field) const {

	assert(field < fields.size());
	if (fields[field].field_name_w.isNull())
		fields[field].field_name_w = fields[field].field->getFieldNameW();
	return fields[field].field_name_w;
}

size_t CMetaInfo::getFieldSize(const size_t field) const {

	assert(field < fields.size());
	return fields[field].field_size;
}

ImmutableString<char> CMetaInfo::getTableName(const size_t field) const {

	assert(field < fields.size());
	ConstIndexIterator p_table = findTableRecord(fields[field].id_table);
	return tables[*p_table].table_name;
}

ImmutableString<wchar_t> CMetaInfo::getTableNameW(const size_t field) const {

	assert(field < fields.size());
	ConstIndexIterator p_table = findTableRecord(fields[field].id_table);
	if (tables[*p_table].table_name_w.isNull())
		tables[*p_table].table_name_w = fields[field].field->getTableNameW();
	return tables[*p_table].table_name_w;
}

bool CMetaInfo::isPrimaryKey(const size_t field) const {

	assert(field < fields.size());
	return fields[field].is_primary_key;
}

template <class FieldPredicate> \
void CMetaInfo::enumeratePrimKey(const id_type table_id, \
									FieldPredicate field_pred) const {
	auto table_prim_keys = findTableKeys(table_id);

	size_t counter = 0;
	while (table_prim_keys.first != table_prim_keys.second) {
		
		field_pred(*(table_prim_keys.first));

		++table_prim_keys.first;
		++counter;
	}
	if (!counter)
		throw XException(0, \
			_T("There are no foreign keys for the table the id field belongs to"));
}

bool CMetaInfo::CFieldRecord::operator==(const CMetaInfo::CFieldRecord &obj) const {

	return this->id == obj.id;
}

bool CMetaInfo::CFieldRecord::operator<(const CMetaInfo::CFieldRecord &obj) const {

	return this->id < obj.id;
}

bool CMetaInfo::CTableRecord::operator==(const CMetaInfo::CTableRecord &obj) const {

	return this->id == obj.id;
}

bool CMetaInfo::CTableRecord::operator<(const CMetaInfo::CTableRecord &obj) const {

	return this->id < obj.id;
}

CMetaInfo::ConstIndexIterator CMetaInfo::findFieldRecord(const char *field_name) const {
	typedef CIndexedTextSearchPredicate<char, IndexType, std::vector<CFieldRecord>, \
										CGetFieldNameByIndex> Pred;
	Pred predFieldName( fields );

	return std::lower_bound(fields_index_name.cbegin(), \
							fields_index_name.cend(), \
							field_name, predFieldName);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findFieldRecord(const id_type id_field) const {
	typedef CIndexedValueSearchPredicate<id_type, IndexType, std::vector<CFieldRecord>, \
											CGetFieldIdByIndex> Pred;
	Pred predFieldId( fields );

	return std::lower_bound(fields_index_id.cbegin(), \
							fields_index_id.cend(), \
							id_field, predFieldId);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findTableRecord(const char *table_name) const {
	typedef CIndexedTextSearchPredicate<char, IndexType, std::vector<CTableRecord>, \
										CGetTableNameByIndex> Pred;
	Pred predTableName( tables );

	return std::lower_bound(tables_index_name.cbegin(), \
							tables_index_name.cend(), \
							table_name, predTableName);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findTableRecord(const id_type id_table) const {
	typedef CIndexedValueSearchPredicate<id_type, IndexType, std::vector<CTableRecord>, \
											CGetTableIdByIndex> Pred;
	Pred predTableId( tables );

	return std::lower_bound(tables_index_id.cbegin(), \
							tables_index_id.cend(), \
							id_table, predTableId);
}

std::pair<CMetaInfo::ConstIndexIterator, CMetaInfo::ConstIndexIterator> \
	CMetaInfo::findTableKeys(const id_type id_table) const {
	typedef CIndexedValueSearchPredicate<id_type, IndexType, \
											std::vector<CFieldRecord>, \
											CGetTableIdFromKeysByIndex> Pred;
	Pred predKeyTableId( fields );

	return std::equal_range(keys_index_table.cbegin(), \
							keys_index_table.cend(), \
							id_table, predKeyTableId);
}

bool CMetaInfo::isFieldRecordFound(const ConstIndexIterator p_field_name, \
									const char *field_name) const {

	return p_field_name != fields_index_name.end() && \
			!strcmp(fields[*p_field_name].field_name.str, field_name);
}

bool CMetaInfo::isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const {
	
	return p_field_id != fields_index_id.end() && \
			(id_field == fields[*p_field_id].id);
}

bool CMetaInfo::isTableRecordFound(const CMetaInfo::ConstIndexIterator p_table_name, \
									const char *table_name) const {

	return p_table_name != tables_index_name.end() && \
			!strcmp(tables[*p_table_name].table_name.str, table_name);
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

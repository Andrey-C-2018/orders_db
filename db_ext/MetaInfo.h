#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/Exception.h>
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

		CKeyRecord() : id_field(0), id_table(0) { }
		CKeyRecord(const CKeyRecord &obj) = default;
		CKeyRecord(CKeyRecord &&obj) = default;
		inline bool operator==(const CKeyRecord &obj) const;
		inline bool operator<(const CKeyRecord &obj) const;
	};

	struct CTableRecord {
		id_type id;
		std::string table_name;

		CTableRecord() : id(0) { }
		CTableRecord(const CTableRecord &obj) = default;
		CTableRecord(CTableRecord &&obj) = default;
		inline bool operator==(const CTableRecord &obj) const;
		inline bool operator<(const CTableRecord &obj) const;
	};

	std::vector<CFieldRecord> fields;
	std::vector<CKeyRecord> keys;
	std::vector<CTableRecord> tables;

	std::vector<IndexType> fields_index_id, fields_index_name, fields_index_order;
	std::vector<IndexType> keys_index_table;
	std::vector<IndexType> tables_index_id, tables_index_name;

	template <class TRegistry, class GetStrByIndex> \
	struct CIndexedTextSearchPredicate {
		TRegistry &reg;

		inline bool operator()(IndexType l, IndexType r) const {

			return getStrByIndex(reg, l).compare(getStrByIndex(reg, r)) < 0;
		}
		inline bool operator()(IndexType l, const std::string &r) const {

			return getStrByIndex(reg, l).compare(r) < 0;
		}
		inline bool operator()(IndexType l, const char *r) const {

			return getStrByIndex(reg, l).compare(r) < 0;
		}
		inline bool operator()(const std::string &l, IndexType r) const {

			return l.compare(getStrByIndex(reg, r)) < 0;
		}
		inline bool operator()(const char *l, IndexType r) const {

			return getStrByIndex(reg, r).compare(l) > 0;
		}
	};

	template <typename TInt, class TRegistry, class GetIntValueByIndex> \
	struct CIndexedIntValueSearchPredicate {
		TRegistry &reg;

		inline bool operator()(IndexType l, IndexType r) const {

			return getStrByIndex(reg, l) < getIntValueByIndex[r];
		}
		inline bool operator()(IndexType l, TInt r) const {

			return getStrByIndex(reg, l) < r;
		}
		inline bool operator()(TInt l, IndexType r) const {

			return l < getStrByIndex(reg, r);
		}
	};

	struct CGetFieldNameByIndex {
		inline const Tstring &operator()(std::vector<CFieldRecord> &fields, \
											const size_t index) const;
	};
	struct CGetFieldIdByIndex {
		inline id_type operator()(std::vector<CFieldRecord> &fields, \
									const size_t index) const;
	};
	struct CGetFieldOrderByIndex {
		inline size_t operator()(std::vector<CFieldRecord> &fields, \
									const size_t index) const;
	};
	struct CGetTableNameByIndex {
		inline const Tstring &operator()(std::vector<CFieldRecord> &tables, \
									const size_t index) const;
	};
	struct CGetTableIdByIndex {
		inline id_type operator()(std::vector<CFieldRecord> &tables, \
									const size_t index) const;
	};
	struct CGetTableIdFromKeysByIndex {
		inline id_type operator()(std::vector<CKeyRecord> &keys, \
									const size_t index) const;
	};

	id_type primary_table_id;
	std::string primary_table_name;

	inline IndexIterator findFieldRecord(const Tchar *field_name);
	inline IndexIterator findFieldRecord(const id_type id_field);
	inline IndexIterator findFieldRecord(const size_t field_order);
	inline IndexIterator findTableRecord(const char *table_name);
	inline IndexIterator findTableRecord(const id_type id_table);
	inline std::pair<IndexIterator, IndexIterator> findTableKeysRecords(const id_type id_table);

	inline bool isFieldRecordFound(const ConstIndexIterator p_field_name, \
									const Tchar *field_name) const;
	inline bool isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const;
	inline bool isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const size_t field_order) const;
	inline bool isTableRecordFound(const ConstIndexIterator p_table_name, \
									const char *table_name) const;
	inline bool isTableRecordFound(const ConstIndexIterator p_table_id, \
									const id_type id_table) const;

	id_type addTableRecord(const char *table_name);

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
	std::shared_ptr<const IDbField> getField(const size_t field_order) const;
	const CDbFieldProperties<Tstring> &getFieldProperties(const size_t field_order) const;

	void setPrimaryTable(const Tchar *table_name);

	void addField(std::shared_ptr<IDbField> field, const int field_order);

	void getUpdateQueryForField(const size_t field_order, std::string &query);
	void getDeleteQuery(std::string &query);

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

CMetaInfo::IndexIterator CMetaInfo::findFieldRecord(const Tchar *field_name) {
	typedef CIndexedTextSearchPredicate<std::vector<CFieldRecord>, \
										CGetFieldNameByIndex> Pred;
	Pred predFieldName{ fields };

	return std::lower_bound(fields_index_name.begin(), \
							fields_index_name.end(), \
							field_name, predFieldName);
}

CMetaInfo::IndexIterator CMetaInfo::findFieldRecord(const id_type id_field) {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CFieldRecord>, \
											CGetFieldIdByIndex> Pred;
	Pred predFieldId{ fields };

	return std::lower_bound(fields_index_id.begin(), \
							fields_index_id.end(), \
							id_field, predFieldId);
}

CMetaInfo::IndexIterator CMetaInfo::findFieldRecord(const size_t field_order) {
	typedef CIndexedIntValueSearchPredicate<size_t, std::vector<CFieldRecord>, \
											CGetFieldOrderByIndex> Pred;
	Pred predFieldOrder{ fields };

	return std::lower_bound(fields_index_id.begin(), \
							fields_index_id.end(), \
							field_order, predFieldOrder);
}

CMetaInfo::IndexIterator CMetaInfo::findTableRecord(const char *table_name) {
	typedef CIndexedTextSearchPredicate<std::vector<CTableRecord>, \
										CGetTableNameByIndex> Pred;
	Pred predTableName{ tables };

	return std::lower_bound(tables_index_name.begin(), \
							tables_index_name.end(), \
							table_name, predTableName);
}

CMetaInfo::IndexIterator CMetaInfo::findTableRecord(const id_type id_table) {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CTableRecord>, \
											CGetTableIdByIndex> Pred;
	Pred predTableId{ tables };

	return std::lower_bound(tables_index_id.begin(), \
							tables_index_id.end(), \
							id_table, predTableId);
}

std::pair<CMetaInfo::IndexIterator, CMetaInfo::IndexIterator> \
	CMetaInfo::findTableKeysRecords(const id_type id_table) {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CKeyRecord>, \
											CGetTableIdFromKeysByIndex> Pred;
	Pred predKeyTableId{ keys };

	return std::equal_range(keys_index_table.begin(), \
							keys_index_table.end(), \
							id_table, predKeyTableId);
}

bool CMetaInfo::isFieldRecordFound(const ConstIndexIterator p_field_name, \
									const Tchar *field_name) const {

	return p_field_name != fields_index_name.end() && \
			!fields[*p_field_name].field_props.field_name.compare(field_name);
}

bool CMetaInfo::isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const {
	
	return p_field_id != fields_index_id.end() && \
			(id_field == fields[*p_field_id].id);
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

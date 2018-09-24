#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <basic/Exception.h>
#include <basic/NumericTypeWrapper.h>
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

	template <typename Tchar_, class Tstring_, \
				class TRegistry, class GetStrByIndex> \
	struct CIndexedTextSearchPredicate {
		const TRegistry &reg;
		GetStrByIndex getStrByIndex;

		inline CIndexedTextSearchPredicate(const TRegistry &reg_) : reg(reg_) { }
		inline bool operator()(IndexType l, IndexType r) const {

			return getStrByIndex(reg, l).compare(getStrByIndex(reg, r)) < 0;
		}
		inline bool operator()(IndexType l, const Tstring_ &r) const {

			return getStrByIndex(reg, l).compare(r) < 0;
		}
		inline bool operator()(IndexType l, const Tchar_ *r) const {

			return getStrByIndex(reg, l).compare(r) < 0;
		}
		inline bool operator()(const Tstring_ &l, IndexType r) const {

			return l.compare(getStrByIndex(reg, r)) < 0;
		}
		inline bool operator()(const Tchar_ *l, IndexType r) const {

			return getStrByIndex(reg, r).compare(l) > 0;
		}
	};

	template <typename TInt, class TRegistry, class GetIntValueByIndex> \
	struct CIndexedIntValueSearchPredicate {
		const TRegistry &reg;
		GetIntValueByIndex getIntValueByIndex;

		inline CIndexedIntValueSearchPredicate(const TRegistry &reg_) : reg(reg_) { }
		inline bool operator()(IndexType l, IndexType r) const {

			return getIntValueByIndex(reg, l) < getIntValueByIndex(reg, r);
		}
		inline bool operator()(IndexType l, TInt r) const {

			return getIntValueByIndex(reg, l) < r;
		}
		inline bool operator()(TInt l, IndexType r) const {

			return l < getIntValueByIndex(reg, r);
		}
	};

	struct CGetFieldNameByIndex {
		inline const Tstring &operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const { return fields[index].field_props.field_name; }
	};
	struct CGetFieldIdByIndex {
		inline id_type operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const {	return fields[index].id; }
	};
	struct CGetFieldOrderByIndex {
		inline size_t operator()(const std::vector<CFieldRecord> &fields, \
						const size_t index) const { return fields[index].order;	}
	};
	struct CGetTableNameByIndex {
		inline const std::string &operator()(const std::vector<CTableRecord> &tables, \
						const size_t index) const {	return tables[index].table_name; }
	};
	struct CGetTableIdByIndex {
		inline id_type operator()(const std::vector<CTableRecord> &tables, \
						const size_t index) const {	return tables[index].id; }
	};
	struct CGetTableIdFromKeysByIndex {
		inline id_type operator()(const std::vector<CKeyRecord> &keys, \
						const size_t index) const {	return keys[index].id_table; }
	};

	id_type primary_table_id;
	std::string primary_table_name;

	inline ConstIndexIterator findFieldRecord(const Tchar *field_name) const;
	inline ConstIndexIterator findFieldRecord(const id_type id_field) const;
	inline ConstIndexIterator findFieldRecord(const size_t field_order) const;
	inline ConstIndexIterator findTableRecord(const char *table_name) const;
	inline ConstIndexIterator findTableRecord(const id_type id_table) const;
	inline std::pair<ConstIndexIterator, ConstIndexIterator> \
					findTableKeysRecords(const id_type id_table) const;

	inline bool isFieldRecordFound(const ConstIndexIterator p_field_name, \
									const Tchar *field_name) const;
	inline bool isFieldRecordFound(const ConstIndexIterator p_field_id, \
									const id_type id_field) const;
	inline bool isFieldRecordFound(const ConstIndexIterator p_field_order, \
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

		ConstIndexIterator p_id_key = findFieldRecord(id);
		assert(isFieldRecordFound(p_id_key, id));

		const CFieldRecord &rec = fields[*p_id_key];
		field_pred(rec.order);

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

CMetaInfo::ConstIndexIterator CMetaInfo::findFieldRecord(const Tchar *field_name) const {
	typedef CIndexedTextSearchPredicate<Tchar, Tstring, \
										std::vector<CFieldRecord>, \
										CGetFieldNameByIndex> Pred;
	Pred predFieldName( fields );

	return std::lower_bound(fields_index_name.cbegin(), \
							fields_index_name.cend(), \
							field_name, predFieldName);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findFieldRecord(const id_type id_field) const {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CFieldRecord>, \
											CGetFieldIdByIndex> Pred;
	Pred predFieldId( fields );

	return std::lower_bound(fields_index_id.cbegin(), \
							fields_index_id.cend(), \
							id_field, predFieldId);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findFieldRecord(const size_t field_order) const {
	typedef CIndexedIntValueSearchPredicate<CNumericTypeWrapper<size_t>, \
											std::vector<CFieldRecord>, \
											CGetFieldOrderByIndex> Pred;
	Pred predFieldOrder( fields );

	return std::lower_bound(fields_index_id.cbegin(), \
							fields_index_id.cend(), \
							CNumericTypeWrapper<size_t>(field_order), predFieldOrder);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findTableRecord(const char *table_name) const {
	typedef CIndexedTextSearchPredicate<char, std::string, \
										std::vector<CTableRecord>, \
										CGetTableNameByIndex> Pred;
	Pred predTableName( tables );

	return std::lower_bound(tables_index_name.cbegin(), \
							tables_index_name.cend(), \
							table_name, predTableName);
}

CMetaInfo::ConstIndexIterator CMetaInfo::findTableRecord(const id_type id_table) const {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CTableRecord>, \
											CGetTableIdByIndex> Pred;
	Pred predTableId( tables );

	return std::lower_bound(tables_index_id.cbegin(), \
							tables_index_id.cend(), \
							id_table, predTableId);
}

std::pair<CMetaInfo::ConstIndexIterator, CMetaInfo::ConstIndexIterator> \
	CMetaInfo::findTableKeysRecords(const id_type id_table) const {
	typedef CIndexedIntValueSearchPredicate<id_type, std::vector<CKeyRecord>, \
											CGetTableIdFromKeysByIndex> Pred;
	Pred predKeyTableId( keys );

	return std::equal_range(keys_index_table.cbegin(), \
							keys_index_table.cend(), \
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

bool CMetaInfo::isFieldRecordFound(const ConstIndexIterator p_field_order, \
									const size_t field_order) const {

	return p_field_order != fields_index_order.end() && \
			(field_order == fields[*p_field_order].order);
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

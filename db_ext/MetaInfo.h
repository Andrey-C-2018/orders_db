#pragma once
#include <memory>
#include <vector>
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
		id_type id_table;
		size_t order;
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

	std::vector<IndexType> fields_index_id;
	std::vector<IndexType> keys_index_table;
	std::vector<IndexType> tables_index_id, tables_index_name;

	id_type primary_table_id;

	inline IndexIterator FindTableRecord(const Tchar *table_name);
	inline IndexIterator FindTableRecord(const int id_table);
	inline ConstIndexIterator FindFieldRecord(const int id_field) const;
	inline std::pair<IndexIterator, IndexIterator> FindTableKeysRecords(const int id_table);

	inline bool IsTableRecordFound(const ConstIndexIterator p_table_name, \
									const Tchar *table_name) const;
	inline bool IsTableRecordFound(const ConstIndexIterator p_table_id, \
									const int id_table) const;
	inline bool IsFieldRecordFound(const ConstIndexIterator p_field_id, \
									const int id_field) const;

	inline int AddNewTableRecord(const Tchar *table_name);
	inline int AddNewFieldRecord(const int id_table, std::shared_ptr<const IDbField> field, \
									const size_t sort_order, const bool hidden);
	inline void AddNewKeyRecord(const int id_field, const int id_table, \
									const bool hidden);

	inline void UpdateIndexes_TablesRecords();
	inline void UpdateIndexes_FieldsRecords();
	inline void UpdateIndexes_KeysRecords();
public:
	CMetaInfo();

	CMetaInfo(const CMetaInfo &obj) = default;
	CMetaInfo(CMetaInfo &&obj) = default;
	CMetaInfo &operator=(const CMetaInfo &obj) = default;
	CMetaInfo &operator=(CMetaInfo &&obj) = default;
	
	inline size_t getFieldsCount() const;
	inline bool empty() const;
	std::shared_ptr<const IDbField> getField(const size_t field) const;
	CDbFieldProperties<Tchar> &getFieldProperties(const size_t field) const;

	void setPrimaryTable(const Tchar *table_name);

	int addField(std::shared_ptr<IDbResultSetMetadata> metadata);

	template <class GetCellPredicate> \
		void getUpdateStr(const size_t updated_field, Tstring &str, \
							GetCellPredicate GetCell);

	template <class GetCellPredicate> \
		void getDeleteStr(std::string &str, GetCellPredicate GetCell);

	virtual ~CMetaInfo();
};


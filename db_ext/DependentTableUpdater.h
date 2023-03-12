#pragma once
#include "DbTable.h"

class CDependentTableUpdater {
	std::shared_ptr<IDbConnection> conn;
	std::string update_query_preamble;
	static std::string constant_modifier;

	std::shared_ptr<const IDbResultSet> master_records;
	CMetaInfo master_meta_info;
	
	struct CFieldsRelation {
		std::string master_field_name;
		std::string dependent_field_name;
		size_t master_field_index;
		size_t dependent_field_index;
	};	
	std::vector<CFieldsRelation> fields_relations;

	std::string dependent_table_name;
	std::shared_ptr<const CDbTable> dependent_db_table;

	inline const CMetaInfo &getDependentMetaInfo() { 
		
		return dependent_db_table->getMetaInfo(); 
	}
public:
	CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
							const char *master_table_name, \
							const char *dependent_table_name, \
							std::shared_ptr<const CDbTable> dependent_table);

	CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
							std::shared_ptr<const IDbResultSet> master_records_, \
							std::shared_ptr<const IDbResultSetMetadata> master_metadata, \
							const char *dependent_table_name, \
							std::shared_ptr<const CDbTable> dependent_table);

	CDependentTableUpdater(const CDependentTableUpdater &obj) = default;
	CDependentTableUpdater(CDependentTableUpdater &&obj) = default;
	CDependentTableUpdater &operator=(const CDependentTableUpdater &obj) = default;
	CDependentTableUpdater &operator=(CDependentTableUpdater &&obj) = default;

	inline std::shared_ptr<const IDbResultSet> getMasterResultSet() const { return master_records; }
	inline const CMetaInfo &getMasterMetaInfo() const { return master_meta_info; }

	void setMasterPrimaryKey(const char *field);
	void AddRelation(const char *master_field, const char *dependent_field);

	static void setQueryConstantModifier(ImmutableString<char> modifier);
	std::shared_ptr<IDbStatement> createDepTableUpdateStmt(const size_t master_record_index);
	
	virtual ~CDependentTableUpdater();
};

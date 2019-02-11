#pragma once;
#include "DbTable.h"

class CDependentTableUpdater {
	std::shared_ptr<IDbConnection> conn;
	std::string update_query_preamble;

	std::shared_ptr<const IDbResultSet> master_records;
	CMetaInfo master_meta_info;
	
	struct CFieldsRelation {
		std::string master_field_name;
		std::string dependent_field_name;
	};	
	std::vector<CFieldsRelation> fields_relations;

	std::shared_ptr<const CDbTable> dependent_db_table;
	inline const CMetaInfo &getDependentMetaInfo() { 
		
		return dependent_db_table->getQuery().getMetaInfo(); 
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

	void SetRelation(const char *master_field, const char *dependent_field);
	std::shared_ptr<IDbStatement> createDepTableUpdateStmt(const size_t master_record_index);
	
	virtual ~CDependentTableUpdater();
};

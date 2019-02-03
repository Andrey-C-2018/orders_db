#pragma once;

class CDependentTableUpdater {
	std::shared_ptr<IDbConnection> conn;

	std::shared_ptr<IDbResultSet> master_records;
	CMetaInfo master_meta_info;
	
	struct CFieldsRelation {
		Tstring master_field_name;
		Tstring dependent_field_name;
	};	
	std::vector<CFieldsRelation> fields_relations;

	std::vector<const CDbTable> dependent_db_table;
	inline CMetaInfo &getDependentMetaInfo();
public:
	CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
		const Tchar *master_table_name, \
		const Tchar *dependent_table_name);

	void SetRelation(const Tchar *master_field, const Tchar *dependent_field);
	std::shared_ptr<IDbStatement> ChooseMasterRecByIndexAndGetUpdateDepRecsStmt(const size_t master_record_index);
	
	virtual ~CDependentTableUpdater();
};

CDependentTableUpdater::CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
			const Tchar *master_table_name, \
			const Tchar *dependent_table_name, \
			std::vector<const CDbTable> dependent_table) : \
				conn(conn_), dependent_db_table(dependent_table) {

	assert(conn_);
	assert(dependent_table);
	
	Tstring query = "SELECT * FROM "; 
	query += master_table_name;
	auto stmt = conn.PrepareQuery(query.c_str());
	master_records = stmt->exec();

	auto fields = stmt->getResultSetMetadata();
	master_meta_info.clearAndAddFields(fields);
}

void CDependentTableUpdater::SetRelation(const Tchar *master_field, const Tchar *dependent_field) {

	assert(dependent_field);
	assert(master_field);

	CFieldsRelation item;
	item.master_field_name = master_field;
	item.dependent_field_name = dependent_field;

	fields_relations.emplace_back(std::move(item));
}

std::shared_ptr<IDbStatement> CDependentTableUpdater::ChooseMasterRecByIndexAndGetUpdateDepRecsStmt(const size_t master_record_index) {
	
	assert(!fields_relations.empty());

	Tstring query;

	query = _T("UPDATE ");
	query += master_table_name;
	query += _T(" m, ");
	query += dependent_table_name;
	query += _T(" d SET ");
	
	for(auto p = fields_relations.cbegin(); p != fields_relations.cend(); ++p){
		
		query += _T("d."); query += p->dependent_field_name;
		query += _T(" = ");
		query += _T("m."); query += p->master_field_name;
		query += _T(", ");
	}
	query.erase(query.end() - 2, query.end());

	size_t parameters_count = 0;
	CMetaInfo &dependent_meta_info = getDependentMetaInfo();
	query += _T(" WHERE ");
	parameters_count = master_meta_info.appendWherePartOfUpdateQuery(query);
	query += _T(" AND ");
	parameters_count += dependent_meta_info.appendWherePartOfUpdateQuery(query);

	std::shared_ptr<IDbStatement> upd_stmt = conn.prepareQuery(query.c_str());

	master_meta_info.bindPrimaryKeyValues(master_records, upd_stmt);
	dependent_meta_info.bindPrimaryKeyValuesWithOffset(parameters_count, dependent_db_table->getResultSet());

	return upd_stmt;
}

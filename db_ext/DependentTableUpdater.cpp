#include "DependentTableUpdater.h"

std::string CDependentTableUpdater::constant_modifier;

CDependentTableUpdater::CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
											const char *master_table_name, \
											const char *dependent_table_name, \
											std::shared_ptr<const CDbTable> dependent_table) : \
											conn(conn_), dependent_db_table(dependent_table) {

	assert(conn_);
	assert(dependent_table);
	assert(dependent_table_name);
	this->dependent_table_name = dependent_table_name;

	std::string query = "SELECT * FROM ";
	query += master_table_name;
	auto stmt = conn->PrepareQuery(query.c_str());
	master_records = stmt->exec();

	auto fields = stmt->getResultSetMetadata();
	master_meta_info.clearAndAddFields(fields);
	master_meta_info.setPrimaryTable(master_table_name);

	query = "UPDATE ";
	query += master_table_name;
	query += " m, ";
	query += dependent_table_name;
	query += " d SET ";
	update_query_preamble = std::move(query);
	if (!constant_modifier.empty()) {
		update_query_preamble += constant_modifier;
		update_query_preamble += ',';
	}
}

CDependentTableUpdater::CDependentTableUpdater(std::shared_ptr<IDbConnection> conn_, \
										std::shared_ptr<const IDbResultSet> master_records_, \
										std::shared_ptr<const IDbResultSetMetadata> master_metadata, \
										const char *dependent_table_name, \
										std::shared_ptr<const CDbTable> dependent_table) : \
								conn(conn_), master_records(master_records_), \
								dependent_db_table(dependent_table) {

	assert(conn_);
	assert(master_records);
	assert(master_metadata);
	assert(dependent_table);
	assert(dependent_table_name);
	this->dependent_table_name = dependent_table_name;

	master_meta_info.clearAndAddFields(master_metadata);
	assert(master_meta_info.getTablesCount() == 1);
	ImmutableString<char> master_table_name = master_meta_info.getTableName(0);
	master_meta_info.setPrimaryTable(master_table_name.str);

	update_query_preamble = "UPDATE ";
	update_query_preamble += master_table_name.str;
	update_query_preamble += " m, ";
	update_query_preamble += dependent_table_name;
	update_query_preamble += " d SET ";
	if (!constant_modifier.empty()) {
		update_query_preamble += constant_modifier;
		update_query_preamble += ',';
	}
}

void CDependentTableUpdater::setQueryConstantModifier(ImmutableString<char> modifier) {

	constant_modifier = modifier.str;
}

void CDependentTableUpdater::AddRelation(const char *master_field, const char *dependent_field) {

	assert(dependent_field);
	assert(master_field);
	const CMetaInfo &dependent_meta_info = getDependentMetaInfo();
	ImmutableString<char> master_table_name = master_meta_info.getTableName(0);

	CFieldsRelation item;
	item.master_field_name = master_field;
	item.dependent_field_name = dependent_field;
	item.master_field_index = master_meta_info.getFieldIndexByName(\
													item.master_field_name.c_str(), \
													master_table_name.str);
	item.dependent_field_index = dependent_meta_info.getFieldIndexByName(\
													item.dependent_field_name.c_str(), \
													dependent_table_name.c_str());
	fields_relations.emplace_back(std::move(item));
}

std::shared_ptr<IDbStatement> \
CDependentTableUpdater::createDepTableUpdateStmt(const size_t master_record_index) {

	assert(!fields_relations.empty());

	std::string query = update_query_preamble;

	for (auto p = fields_relations.cbegin(); p != fields_relations.cend(); ++p) {

		query += "d."; query += p->dependent_field_name;
		query += " = ";
		query += "m."; query += p->master_field_name;
		query += ", ";
	}
	query.erase(query.end() - 2, query.end());

	size_t master_primkey_params_count = 0;
	const CMetaInfo &dependent_meta_info = getDependentMetaInfo();
	query += " WHERE ";
	master_primkey_params_count = master_meta_info.appendWherePartOfUpdateQuery(query, "m");
	query += " AND ";
	dependent_meta_info.appendWherePartOfUpdateQuery(dependent_table_name.c_str(), query, "d");

	std::shared_ptr<IDbStatement> upd_stmt = conn->PrepareQuery(query.c_str());

	master_records->gotoRecord(master_record_index);
	auto dependent_result_set = dependent_db_table->getResultSet();

	master_meta_info.bindPrimaryKeyValues(fields_relations[0].master_field_index, \
											master_records, upd_stmt);
	dependent_meta_info.bindPrimaryKeyValuesWithOffset(\
											fields_relations[0].dependent_field_index, \
														master_primkey_params_count, \
														dependent_result_set, upd_stmt);
	return upd_stmt;
}

CDependentTableUpdater::~CDependentTableUpdater() { }
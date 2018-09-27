#include "DbTable.h"

inline void bindValueAsTString(std::shared_ptr<const IDbField> field, \
								std::shared_ptr<IDbBindingTarget> binding_target, \
								const size_t param_no, \
								const char *value) {

	field->bindValueAsString(binding_target, param_no, value);
}

inline void bindValueAsTString(std::shared_ptr<const IDbField> field, \
								std::shared_ptr<IDbBindingTarget> binding_target, \
								const size_t param_no, \
								const wchar_t *value) {

	field->bindValueAsWString(binding_target, param_no, value);
}

//****************************************************

CDbTable::CDbTable(CQuery query_) : curr_record(0), query(std::move(query_)){

	result_set = query.exec();

	on_size_changed_handlers.reserve(DEF_ON_CHANGE_HANDLERS_COUNT);
}

bool CDbTable::empty() const {

	bool no_fields = query.getMetaInfo().empty();
	return no_fields && !result_set->getRecordsCount();
}

size_t CDbTable::GetFieldsCount() const{

	return query.getMetaInfo().getFieldsCount();
}

size_t CDbTable::GetRecordsCount() const{

	return result_set->getRecordsCount();
}

void CDbTable::ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler){

	on_size_changed_handlers.push_back(handler);
}

void CDbTable::AddField(const size_t max_field_len, const Tchar *field_name){

	throw XException(0, _T("Not supported"));
}

void CDbTable::AddRecord(){
	size_t records_count = result_set->getRecordsCount();

	conn->ExecScalarQuery(insert_str.c_str());
	reload();

	for (auto &handler : on_size_changed_handlers)
		handler->OnSizeChanged(query.getMetaInfo().getFieldsCount(), \
					records_count);
}

ImmutableString<Tchar> CDbTable::GetCellAsString(const size_t field, const size_t record) const{

	result_set->gotoRecord(record);
	auto field_ptr = query.getMetaInfo().getField(field);
	return field_ptr->getValueAsImmutableString(result_set);
}

void CDbTable::SetCell(const size_t field, const size_t record, const Tchar *value){

	result_set->gotoRecord(record);
	auto update_stmt = query.getUpdateStmt(field, result_set);

	auto updated_field_ptr = query.getMetaInfo().getField(field);
	bindValueAsTString(updated_field_ptr, update_stmt, 0, value);
	update_stmt->execScalar();

	reload();
}

void CDbTable::reload(){

	result_set = query.exec();
}

CDbTable::~CDbTable(){ }

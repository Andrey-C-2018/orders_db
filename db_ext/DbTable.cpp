#include "DbTable.h"

CDbTableException::CDbTableException(const int err_code, \
										const Tchar *err_descr) : \
										XException(err_code, err_descr) { }

CDbTableException::CDbTableException(const CDbTableException &obj) : \
										XException(obj) { }

CDbTableException::~CDbTableException() { }

//****************************************************************************

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

inline ImmutableString<char> getCellAsString(std::shared_ptr<IDbResultSet> result_set, \
												std::shared_ptr<const IDbField> field, \
												char type_hint) {

	return field->getValueAsImmutableString(result_set);
}

inline ImmutableString<wchar_t> getCellAsString(std::shared_ptr<IDbResultSet> result_set, \
												std::shared_ptr<const IDbField> field, \
												wchar_t type_hint) {

	return field->getValueAsImmutableWString(result_set);
}

inline ImmutableString<char> getFieldName(const CMetaInfo &meta_info, \
								const size_t field, char type_hint) {

	return meta_info.getFieldName(field);
}

inline ImmutableString<wchar_t> getFieldName(const CMetaInfo &meta_info, \
									const size_t field, wchar_t type_hint) {

	return meta_info.getFieldNameW(field);
}

//****************************************************

CDbTable::CDbTable(std::shared_ptr<IDbConnection> conn_, CQuery query_) : \
					conn(conn_), query(std::move(query_)), curr_record(0){

	result_set = query.exec();
}

bool CDbTable::empty() const {

	bool no_fields = query.getMetaInfo().empty();
	bool no_records = result_set->empty();
	return no_fields || no_records;
}

size_t CDbTable::GetFieldsCount() const{

	return query.getMetaInfo().getFieldsCount();
}

size_t CDbTable::GetRecordsCount() const{

	return result_set->getRecordsCount();
}

void CDbTable::ConnectEventsHandler(ITableEventsHandlerPtr handler) {

	event_handlers.ConnectEventsHandler(handler);
}

void CDbTable::DisconnectEventsHandler(ITableEventsHandlerPtr handler) {

	event_handlers.DisconnectEventsHandler(handler);
}

void CDbTable::ConnectDbEventsHandler(IDbTableEvtHandlerPtr handler) {

	db_event_handlers.ConnectEventsHandler(handler);
}

void CDbTable::DisconnectDbEventsHandler(IDbTableEvtHandlerPtr handler) {

	db_event_handlers.DisconnectEventsHandler(handler);
}

void CDbTable::AddField(const size_t max_field_len, const Tchar *field_name){

	throw CDbTableException(CDbTableException::E_WRONG_OPERATION, \
							_T("Adding fields using CDbTable is not supported"));
}

void CDbTable::AddRecord(){

	throw CDbTableException(CDbTableException::E_WRONG_OPERATION, \
							_T("Adding records using CDbTable is not supported"));
}

size_t CDbTable::GetFieldMaxLengthInChars(const size_t field) const {

	return query.getMetaInfo().getFieldSize(field);
}

const Tchar *CDbTable::GetFieldName(const size_t field) const {

	return getFieldName(query.getMetaInfo(), field, Tchar()).str;
}

ImmutableString<Tchar> CDbTable::GetFieldNameAsImmutableStr(const size_t field) const {

	return getFieldName(query.getMetaInfo(), field, Tchar());
}

ImmutableString<Tchar> CDbTable::GetCellAsString(const size_t field, const size_t record) const{

	result_set->gotoRecord(record);
	auto field_ptr = query.getMetaInfo().getField(field);
	return getCellAsString(result_set, field_ptr, Tchar());
}

void CDbTable::SetCell(const size_t field, const size_t record, const Tchar *value){

	result_set->gotoRecord(record);
	std::shared_ptr<IDbStatement> update_stmt;

	auto updated_field_ptr = query.getMetaInfo().getField(field);
	update_stmt = query.getUpdateStmt(field, result_set);
	if (value && value[0] != _T('\0'))
		bindValueAsTString(updated_field_ptr, update_stmt, 0, value);
	else
		update_stmt->bindNull(0);
	
	update_stmt->execScalar();

	rereadQueryContents();
}

void CDbTable::removeCurrentRecord() {

	result_set->gotoRecord(curr_record);
	auto del_stmt = query.getDeleteStmt(result_set);

	del_stmt->execScalar();
	rereadQueryContents();
}

void CDbTable::reload(){

	rereadQueryContents();
}

void CDbTable::reload(std::shared_ptr<IDbStatement> stmt) {

	query.setStmtWithTheSameMetaInfo(stmt);
	result_set = query.exec();

	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);
	db_event_handlers.OnRecordsCountChanged(records_count);
}

CDbTable::~CDbTable(){ }

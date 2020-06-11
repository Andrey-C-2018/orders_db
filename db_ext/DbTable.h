#pragma once
#include <basic/Exception.h>
#include <table/ITable.h>
#include <db/IDbConnection.h>
#include <db/IDbResultSet.h>
#include <table/EventsHandlersContainer.h>
#include "DbEventsHandlersContainer.h"
#include "Query.h"

class CDbTableException : public XException {
public:
	enum {
		E_WRONG_OPERATION = 1
	};
	CDbTableException(const int err_code, const Tchar *err_descr);
	CDbTableException(const CDbTableException &obj);
	CDbTableException(CDbTableException &&obj) = default;
	~CDbTableException();
};

typedef std::shared_ptr<IDbTableEventsHandler> IDbTableEvtHandlerPtr;

class CDbTable : public ITable{
	std::shared_ptr<IDbConnection> conn;
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;

	size_t curr_record;
	bool rev_sorting_order;

	CDbEventsHandlersContainer db_event_handlers;
	CEventsHandlersContainer<ITableEventsHandler> event_handlers;
	inline void rereadQueryContents();

	void ConnectEventsHandler(ITableEventsHandlerPtr handler) override;
	void DisconnectEventsHandler(ITableEventsHandlerPtr handler) override;
public:
	CDbTable(std::shared_ptr<IDbConnection> conn_, CQuery query_, \
				const bool rev_sorting_order_ = false);

	inline size_t getCurrentRecordNo() const;
	inline void setCurrentRecordNo(const size_t rec_no);
	inline void gotoCurrentRecord();

	inline const CQuery &getQuery() const;
	inline std::shared_ptr<const IDbResultSet> getResultSet() const;
	inline std::shared_ptr<IDbBindingTarget> getBindingTarget();
	inline void setPrimaryTableForQuery(const char *table_name);
	inline void markFieldAsPrimaryKeyByIndex(const size_t field);
	inline void markFieldAsPrimaryKey(const char *field_name, \
										const char *table_name);
	inline void addPrimaryKeyAsRef(const char *field_name, const char *table_name, \
									const char *referenved_field_name, \
									const char *referenced_table_name);

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;

	void ConnectDbEventsHandler(IDbTableEvtHandlerPtr handler);
	void DisconnectDbEventsHandler(IDbTableEvtHandlerPtr handler);

	void AddField(const size_t max_field_len, const Tchar *field_name) override;
	void AddRecord() override;
	void removeCurrentRecord();

	size_t GetFieldMaxLengthInChars(const size_t field) const override;
	const Tchar *GetFieldName(const size_t field) const override;
	ImmutableString<Tchar> GetFieldNameAsImmutableStr(const size_t field) const override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const override;
	void SetCell(const size_t field, const size_t record, const Tchar *value) override;

	inline void executeScalarStmt(std::shared_ptr<IDbStatement> stmt);

	inline bool invertSortingOrder();

	void reload();
	void reload(std::shared_ptr<IDbStatement> stmt);
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

void CDbTable::setCurrentRecordNo(const size_t rec_no) {

	curr_record = rec_no;
	db_event_handlers.OnCurrRecordNoChanged(curr_record);
}

void CDbTable::gotoCurrentRecord() {

	result_set->gotoRecord(curr_record);
}

const CQuery &CDbTable::getQuery() const {

	return query;
}

std::shared_ptr<const IDbResultSet> CDbTable::getResultSet() const {

	return result_set;
}

std::shared_ptr<IDbBindingTarget> CDbTable::getBindingTarget() {

	return query.getBindingTarget();
}

void CDbTable::setPrimaryTableForQuery(const char *table_name) {

	query.setPrimaryTable(table_name);
}

void CDbTable::markFieldAsPrimaryKeyByIndex(const size_t field) {

	query.markFieldAsPrimaryKey(field);
}

void CDbTable::markFieldAsPrimaryKey(const char *field_name, \
										const char *table_name) {
	
	size_t field = query.getMetaInfo().getFieldIndexByName(field_name, table_name);
	query.markFieldAsPrimaryKey(field);
}

void CDbTable::addPrimaryKeyAsRef(const char *field_name, const char *table_name, \
									const char *referenved_field_name, \
									const char *referenced_table_name) {

	query.addPrimaryKeyAsRef(field_name, table_name, \
							referenved_field_name, referenced_table_name);
}

void CDbTable::executeScalarStmt(std::shared_ptr<IDbStatement> stmt) {

	assert(stmt);
	if (stmt->execScalar())	reload();
}

void CDbTable::rereadQueryContents() {

	result_set->reload();

	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);
	db_event_handlers.OnRecordsCountChanged(records_count);
}

bool CDbTable::invertSortingOrder() {

	rev_sorting_order = !rev_sorting_order;
	query.setSortingOrder(rev_sorting_order);
	result_set->reload();

	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);

	return rev_sorting_order;
}
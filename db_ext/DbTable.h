#pragma once
#include <basic/Exception.h>
#include <table/ITable.h>
#include <db/IDbConnection.h>
#include <db/IDbResultSet.h>
#include <table/EventsHandlersContainer.h>
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

class CDbTable : public ITable{
	std::shared_ptr<IDbConnection> conn;
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;

	size_t curr_record;

	CEventsHandlersContainer event_handlers;
	inline void rereadQueryContents();
public:
	CDbTable(std::shared_ptr<IDbConnection> conn_, CQuery query_);

	inline size_t getCurrentRecordNo() const;
	inline void setCurrentRecordNo(const size_t rec_no);
	inline void gotoCurrentRecord();

	inline const CQuery &getQuery() const;
	inline std::shared_ptr<const IDbResultSet> getResultSet() const;
	inline void setPrimaryTableForQuery(const char *table_name);
	inline void markFieldAsPrimaryKeyByIndex(const size_t field);
	inline void markFieldAsPrimaryKey(const char *field_name);

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	void ConnectEventsHandler(ITableEventsHandlerPtr handler) override;
	void DisconnectEventsHandler(ITableEventsHandlerPtr handler) override;

	void AddField(const size_t max_field_len, const Tchar *field_name) override;
	void AddRecord() override;

	size_t GetFieldMaxLengthInChars(const size_t field) const override;
	const Tchar *GetFieldName(const size_t field) const override;
	ImmutableString<Tchar> GetFieldNameAsImmutableStr(const size_t field) const override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const override;
	void SetCell(const size_t field, const size_t record, const Tchar *value) override;

	inline void executeScalarStmt(std::shared_ptr<IDbStatement> stmt);

	void reload();
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

void CDbTable::setCurrentRecordNo(const size_t rec_no) {

	curr_record = rec_no;
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

void CDbTable::setPrimaryTableForQuery(const char *table_name) {

	query.setPrimaryTable(table_name);
}

void CDbTable::markFieldAsPrimaryKeyByIndex(const size_t field) {

	query.markFieldAsPrimaryKey(field);
}

void CDbTable::markFieldAsPrimaryKey(const char *field_name) {
	
	size_t field = query.getMetaInfo().getFieldIndexByName(field_name);
	query.markFieldAsPrimaryKey(field);
}

void CDbTable::executeScalarStmt(std::shared_ptr<IDbStatement> stmt) {

	assert(stmt);
	if (stmt->execScalar())
		reload();
}

void CDbTable::rereadQueryContents() {

	result_set = query.exec();
	//result_set->reload();
	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);
}
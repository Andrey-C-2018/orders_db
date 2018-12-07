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
public:
	CDbTable(std::shared_ptr<IDbConnection> conn_, CQuery query_);

	inline size_t getCurrentRecordNo() const;

	inline const CQuery &getQuery() const;

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

	void reload();
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

const CQuery &CDbTable::getQuery() const {

	return query;
}

#pragma once
#include <basic/Exception.h>
#include <table/ITable.h>
#include <db/IDbConnection.h>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "MetaInfo.h"
#include "ScalarQueryCache.h"
#include "DbEventsHandlersContainer.h"
#include "RevDbResultSet.h"

class CDbTableException : public XException {
public:
	enum {
		E_WRONG_OPERATION = 1
	};
	CDbTableException(const int err_code, const Tchar *err_descr);
	CDbTableException(const CDbTableException &obj);
	CDbTableException(CDbTableException &&obj) = default;
	virtual ~CDbTableException();
};

struct IDbTableEventsHandler;
struct ITableEventsHandler;

typedef std::shared_ptr<IDbTableEventsHandler> IDbTableEvtHandlerPtr;

class CDbTable : public ITable{
	std::shared_ptr<IDbConnection> conn;
	std::shared_ptr<IDbStatement> stmt;

	CMetaInfo meta_info;
	ScalarQueryCache scalar_query_cache;
	std::shared_ptr<CRevDbResultSet> result_set;

	size_t curr_record;
	bool rev_sorting_order;

	CDbEventsHandlersContainer db_event_handlers;
	CEventsHandlersContainer<ITableEventsHandler> event_handlers;
	inline void rereadQueryContents();

	void ConnectEventsHandler(ITableEventsHandlerPtr handler) override;
	void DisconnectEventsHandler(ITableEventsHandlerPtr handler) override;

public:
	CDbTable(std::shared_ptr<IDbConnection> conn_, \
				std::shared_ptr<IDbStatement> stmt_, \
				bool rev_sorting_order_ = false);

	CDbTable(const CDbTable& obj) = delete;
	CDbTable(CDbTable&& obj) = default;
	CDbTable &operator=(const CDbTable& obj) = delete;
	CDbTable &operator=(CDbTable&& obj) = delete;

	inline size_t getCurrentRecordNo() const;
	inline void setCurrentRecordNo(size_t rec_no);
	inline void gotoCurrentRecord();

	inline const CMetaInfo &getMetaInfo() const;

	inline std::shared_ptr<const IDbResultSet> getResultSet() const;
	inline std::shared_ptr<IDbBindingTarget> getBindingTarget();
	inline void setPrimaryTableForQuery(const char *table_name);
	inline void markFieldAsPrimaryKeyByIndex(size_t field);
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

	inline void executeScalarStmt(std::shared_ptr<IDbStatement> scalar_stmt);

	inline bool invertSortingOrder();

	void reload();
	void reload(std::shared_ptr<IDbStatement> stmt);
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

void CDbTable::setCurrentRecordNo(size_t rec_no) {

	curr_record = rec_no;
	db_event_handlers.OnCurrRecordNoChanged(curr_record);
}

void CDbTable::gotoCurrentRecord() {

	result_set->gotoRecord(curr_record);
}

const CMetaInfo &CDbTable::getMetaInfo() const {

	return meta_info;
}

std::shared_ptr<const IDbResultSet> CDbTable::getResultSet() const {

	return result_set;
}

std::shared_ptr<IDbBindingTarget> CDbTable::getBindingTarget() {

	return stmt;
}

void CDbTable::setPrimaryTableForQuery(const char *table_name) {

	meta_info.setPrimaryTable(table_name);
}

void CDbTable::markFieldAsPrimaryKeyByIndex(size_t field) {

	meta_info.markFieldAsPrimaryKey(field);
}

void CDbTable::markFieldAsPrimaryKey(const char *field_name, \
										const char *table_name) {
	
	size_t field = meta_info.getFieldIndexByName(field_name, table_name);
	meta_info.markFieldAsPrimaryKey(field);
}

void CDbTable::addPrimaryKeyAsRef(const char *field_name, const char *table_name, \
									const char *referenced_field_name, \
									const char *referenced_table_name) {

	meta_info.addInvisibleFieldAsRef(field_name, table_name, \
									referenced_field_name,
									 referenced_table_name, true);
}

void CDbTable::executeScalarStmt(std::shared_ptr<IDbStatement> scalar_stmt) {

	assert(scalar_stmt);
	if (scalar_stmt->execScalar()) reload();
}

void CDbTable::rereadQueryContents() {

	result_set->reload();

	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);
	db_event_handlers.OnRecordsCountChanged(records_count);
}

bool CDbTable::invertSortingOrder() {

	assert(result_set);

	rev_sorting_order = !rev_sorting_order;
	result_set->setSortingOrder(rev_sorting_order);
	result_set->reload();

	size_t records_count = result_set->getRecordsCount();
	event_handlers.OnRecordsCountChanged(records_count);

	return rev_sorting_order;
}
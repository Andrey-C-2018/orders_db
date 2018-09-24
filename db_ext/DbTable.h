#pragma once
#include <vector>
#include <basic/Exception.h>
#include <table/ITable.h>
#include <db/IDbResultSet.h>
#include "Query.h"

class CDbTable : public ITable{
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;

	size_t curr_record;

	enum Defaults {
		DEF_ON_CHANGE_HANDLERS_COUNT = 5
	};
	std::vector<ITableOnSizeChangedHandlerPtr> on_size_changed_handlers;
public:
	CDbTable(CQuery query_);

	inline size_t getCurrentRecordNo() const;

	const CQuery &getQuery() const;

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) override;

	void SetInsertString(const Tchar *insert_str);
	void AddField(const int field_size, const Tchar *field_name) override;
	void AddRecord() override;

	size_t GetFieldSize(const size_t field) const override;
	int GetSizesSumm() const override;
	size_t GetFieldName(const size_t field) const override;

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

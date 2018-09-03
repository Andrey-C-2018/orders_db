#pragma once
#include <table/ITable.h>
#include <db/IDbResultSet.h>
#include "Query.h"

class CDbTable : public ITable{
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;

	size_t curr_record;

	class CFieldsProperties;

	std::shared_ptr<CFieldsProperties> fields_props;

	enum Defaults {
		DEF_ON_CHANGE_HANDLERS_COUNT = 5
	};
	std::vector<ITableOnSizeChangedHandlerPtr> on_size_changed_handlers;
public:
	CDbTable(CQuery query_);

	inline size_t getCurrentRecordNo() const;

	const CQuery &getQuery() const;
	std::shared_ptr<const IFieldsProperties> GetFieldsProperties() const override;

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) override;

	void AddField(const int field_size, const Tchar *field_name) override;
	void SetInsertString(ImmutableString<Tchar> insert_str);
	void AddRecord() override;
	void SetFieldSize(const size_t field, const int field_size) override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const override;
	void SetCell(const size_t field, const size_t record, const Tchar *value) override;

	void reload();
	virtual ~CDbTable();
};

//*****************************************************

size_t CDbTable::getCurrentRecordNo() const{

	return curr_record;
}

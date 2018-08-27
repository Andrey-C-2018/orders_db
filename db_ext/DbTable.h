#pragma once
#include <table/ITable.h>
#include <db/IDbResultSet.h>
#include "Query.h"

class CDbTable : public ITable{
	CQuery query;
	std::shared_ptr<IDbResultSet> result_set;
public:
	CDbTable(CQuery query_);

	size_t getCurrentRecordNo() const;

	const CQuery &getQuery() const;
	CQuery &getQuery();

	void SetCell(const size_t field, const size_t record, \
					const Tchar *value) final;

	virtual ~CDbTable();
};


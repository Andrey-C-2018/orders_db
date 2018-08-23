#pragma once
#include <memory>
#include <table/ITable.h>
#include <db/IDbResultSet.h>

class CDbTable : public ITable{
	std::shared_ptr<IDbResultSet> result_set;
public:
	CDbTable();

	size_t GetCurrentRecordNo() const;

	void SetCell(const size_t field, const size_t record, \
					const Tchar *value) final;

	virtual ~CDbTable();
};


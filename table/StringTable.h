#pragma once
#include <vector>
#include <basic/Exception.h>
#include "ITable.h"

class CStringTable : public ITable {
	enum Defaults {
		DEF_ON_CHANGE_HANDLERS_COUNT = 5
	};
	typedef std::vector<Tstring> CTextVector;
	typedef std::vector<CTextVector> CTextTable;

	CTextTable text_table;
	std::vector<size_t> fields_lengthes;
	std::vector<ITableOnSizeChangedHandlerPtr> on_size_changed_handlers;
public:
	CStringTable();

	bool empty() const override;
	size_t GetFieldsCount() const override;
	size_t GetRecordsCount() const override;
	void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) override;

	void AddField(const size_t max_field_len, const Tchar *field_name) override;
	void AddRecord() override;

	size_t GetFieldMaxLengthInChars(const size_t field) const override;
	const Tchar *GetFieldName(const size_t field) const override;

	ImmutableString<Tchar> GetCellAsString(const size_t field, \
									const size_t record) const override;
	void SetCell(const size_t field, const size_t record, \
					const Tchar *value) override;

	virtual ~CStringTable();
};
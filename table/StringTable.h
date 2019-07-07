#pragma once
#include <basic/Exception.h>
#include "ITable.h"
#include "EventsHandlersContainer.h"

class CStringTable : public ITable {
	enum Defaults {
		DEF_FIELDS_COUNT = 10
	};
	typedef std::vector<Tstring> CTextVector;
	typedef std::vector<CTextVector> CTextTable;

	CTextTable text_table;

	struct CField {
		size_t max_size_in_chars;
		Tstring field_name;
	};
	std::vector<CField> fields;
	CEventsHandlersContainer<ITableEventsHandler> event_handlers;
public:
	CStringTable();

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

	ImmutableString<Tchar> GetCellAsString(const size_t field, \
									const size_t record) const override;
	void SetCell(const size_t field, const size_t record, \
					const Tchar *value) override;

	virtual ~CStringTable();
};
#pragma once
#include <memory>
#include <basic/tstring.h>
#include <basic/ImmutableString.h>

struct ITableEventsHandler {
	virtual void OnFieldsCountChanged(const size_t new_fields_count) = 0;
	virtual void OnRecordsCountChanged(const size_t new_records_count) = 0;
	virtual void OnTableReset() = 0;
	virtual ~ITableEventsHandler(){ }
};

typedef std::shared_ptr<ITableEventsHandler> ITableEventsHandlerPtr;

struct ITable {
	virtual bool empty() const = 0;
	virtual size_t GetFieldsCount() const = 0;
	virtual size_t GetRecordsCount() const = 0;
	virtual void ConnectEventsHandler(ITableEventsHandlerPtr handler) = 0;
	virtual void DisconnectEventsHandler(ITableEventsHandlerPtr handler) = 0;

	virtual void AddField(const size_t max_field_len, const Tchar *field_name) = 0;
	virtual void AddRecord() = 0;

	virtual size_t GetFieldMaxLengthInChars(const size_t field) const = 0;
	virtual const Tchar *GetFieldName(const size_t field) const = 0;
	virtual ImmutableString<Tchar> GetFieldNameAsImmutableStr(const size_t field) const = 0;
	
	virtual ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const = 0;
	virtual void SetCell(const size_t field, const size_t record, \
							const Tchar *value) = 0;
	virtual ~ITable(){ }
};

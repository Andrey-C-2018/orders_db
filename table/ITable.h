#pragma once
#include <memory>
#include <basic/tstring.h>
#include <basic/ImmutableString.h>

struct ITableOnSizeChangedHandler {
	virtual void OnSizeChanged(const size_t new_fields_count, \
								const size_t new_records_count) = 0;
	virtual ~ITableOnSizeChangedHandler(){ }
};

typedef std::shared_ptr<ITableOnSizeChangedHandler> ITableOnSizeChangedHandlerPtr;

struct ITable {
	virtual bool empty() const = 0;
	virtual size_t GetFieldsCount() const = 0;
	virtual size_t GetRecordsCount() const = 0;
	virtual void ConnectOnSizeChangedHandler(ITableOnSizeChangedHandlerPtr handler) = 0;

	virtual void AddField(const int field_size, const Tchar *field_name) = 0;
	virtual void AddRecord() = 0;

	virtual size_t GetFieldSize(const size_t field) const = 0;
	virtual int GetSizesSumm() const = 0;
	virtual size_t GetFieldName(const size_t field) const = 0;

	virtual ImmutableString<Tchar> GetCellAsString(const size_t field, const size_t record) const = 0;
	virtual void SetCell(const size_t field, const size_t record, \
							const Tchar *value) = 0;
	virtual ~ITable(){ }
};

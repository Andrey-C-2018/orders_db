#pragma once
#include <table/ITable.h>

struct IDbTableEventsHandler : public ITableEventsHandler {
	virtual void OnCurrRecordNoChanged(const size_t new_record) = 0;
	virtual ~IDbTableEventsHandler() { }
};
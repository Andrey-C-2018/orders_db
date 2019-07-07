#pragma once
#include <table/EventsHandlersContainer.h>
#include "IDbTableEventsHandler.h"

class CDbEventsHandlersContainer : public CEventsHandlersContainer<IDbTableEventsHandler> {
public:
	CDbEventsHandlersContainer() { }
	inline void OnCurrRecordNoChanged(const size_t new_record);
	virtual ~CDbEventsHandlersContainer() { }
};

//*****************************************************

void CDbEventsHandlersContainer::OnCurrRecordNoChanged(const size_t new_record) {

	for (auto & handler : event_handlers)
		handler->OnCurrRecordNoChanged(new_record);
}
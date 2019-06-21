#include <algorithm>
#include "ITable.h"
#include "EventsHandlersContainer.h"

CEventsHandlersContainer::CEventsHandlersContainer(){

	event_handlers.reserve(DEF_EVENT_HANDLERS_COUNT);
}

void CEventsHandlersContainer::ConnectEventsHandler(ITableEventsHandlerPtr handler) {

	assert(handler);
	event_handlers.push_back(handler);
}

void CEventsHandlersContainer::DisconnectEventsHandler(ITableEventsHandlerPtr handler) {

	auto p = std::find(event_handlers.begin(), event_handlers.end(), handler);
	assert(p != event_handlers.end());

	event_handlers.erase(p);
}

void CEventsHandlersContainer::OnFieldsCountChanged(const size_t new_fields_count) {

	for (auto & handler : event_handlers)
		handler->OnFieldsCountChanged(new_fields_count);
}

void CEventsHandlersContainer::OnRecordsCountChanged(const size_t new_records_count) {

	for (auto & handler : event_handlers)
		handler->OnRecordsCountChanged(new_records_count);
}

void CEventsHandlersContainer::OnTableReset() {

	for (auto & handler : event_handlers)
		handler->OnTableReset();
}

CEventsHandlersContainer::~CEventsHandlersContainer(){ }

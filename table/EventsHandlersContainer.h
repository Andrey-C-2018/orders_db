#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "ITable.h"

template <class TEventsHandler> \
class CEventsHandlersContainer {
	enum Defaults {
		DEF_EVENT_HANDLERS_COUNT = 5
	};

protected:
	typedef std::shared_ptr<TEventsHandler> TEventsHandlerPtr;

	std::vector<TEventsHandlerPtr> event_handlers;
public:
	CEventsHandlersContainer();

	void ConnectEventsHandler(TEventsHandlerPtr handler);
	void DisconnectEventsHandler(TEventsHandlerPtr handler);

	void OnFieldsCountChanged(const size_t new_fields_count);
	void OnRecordsCountChanged(const size_t new_records_count);
	void OnTableReset();

	virtual ~CEventsHandlersContainer();
};

//*****************************************************

template <class TEventsHandler> \
CEventsHandlersContainer<TEventsHandler>::CEventsHandlersContainer() {

	event_handlers.reserve(DEF_EVENT_HANDLERS_COUNT);
}

template <class TEventsHandler> \
void CEventsHandlersContainer<TEventsHandler>::ConnectEventsHandler(TEventsHandlerPtr handler) {

	assert(handler);
	event_handlers.push_back(handler);
}

template <class TEventsHandler> \
void CEventsHandlersContainer<TEventsHandler>::DisconnectEventsHandler(TEventsHandlerPtr handler) {

	auto p = std::find(event_handlers.begin(), event_handlers.end(), handler);
	assert(p != event_handlers.end());

	event_handlers.erase(p);
}

template <class TEventsHandler> \
void CEventsHandlersContainer<TEventsHandler>::OnFieldsCountChanged(const size_t new_fields_count) {

	for (auto & handler : event_handlers)
		handler->OnFieldsCountChanged(new_fields_count);
}

template <class TEventsHandler> \
void CEventsHandlersContainer<TEventsHandler>::OnRecordsCountChanged(const size_t new_records_count) {

	for (auto & handler : event_handlers)
		handler->OnRecordsCountChanged(new_records_count);
}

template <class TEventsHandler> \
void CEventsHandlersContainer<TEventsHandler>::OnTableReset() {

	for (auto & handler : event_handlers)
		handler->OnTableReset();
}

template <class TEventsHandler> \
CEventsHandlersContainer<TEventsHandler>::~CEventsHandlersContainer() { }

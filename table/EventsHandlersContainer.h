#pragma once
#include <vector>
#include <memory>

struct ITableEventsHandler;

class CEventsHandlersContainer {
	enum Defaults {
		DEF_EVENT_HANDLERS_COUNT = 5
	};
	typedef std::shared_ptr<ITableEventsHandler> ITableEventsHandlerPtr;

	std::vector<ITableEventsHandlerPtr> event_handlers;
public:
	CEventsHandlersContainer();

	void ConnectEventsHandler(ITableEventsHandlerPtr handler);
	void DisconnectEventsHandler(ITableEventsHandlerPtr handler);

	void OnFieldsCountChanged(const size_t new_fields_count);
	void OnRecordsCountChanged(const size_t new_records_count);
	void OnTableReset();

	virtual ~CEventsHandlersContainer();
};


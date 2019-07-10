#pragma once
#include <db_controls/DbGridEventsHandler.h>

class CAdvocatsListEvtHandler : public CDbGridEventsHandler {
	
public:
	CAdvocatsListEvtHandler(std::shared_ptr<CDbTable> db_table);
	virtual ~CAdvocatsListEvtHandler();
};


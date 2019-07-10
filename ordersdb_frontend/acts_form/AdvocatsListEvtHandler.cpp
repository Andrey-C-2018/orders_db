#include "AdvocatsListEvtHandler.h"

CAdvocatsListEvtHandler::CAdvocatsListEvtHandler(std::shared_ptr<CDbTable> db_table) : \
													CDbGridEventsHandler(db_table) { }

CAdvocatsListEvtHandler::~CAdvocatsListEvtHandler() { }

#pragma once
#include "ICellEventHandler.h"

struct IGridEventsHandler : public ICellEventHandler {

	virtual void OnActiveCellLocationChanged(const size_t new_field, \
												const size_t new_record) = 0;
	virtual ~IGridEventsHandler() { }
};
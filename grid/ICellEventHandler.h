#pragma once
#include "IGridCellWidget.h"

struct ICellEventHandler {

	virtual bool OnCellChanged(IGridCellWidget *cell_widget) = 0;
	virtual bool OnCellChangedIndirectly(IGridCellWidget *cell_widget) = 0;
	virtual ~ICellEventHandler() { }
};
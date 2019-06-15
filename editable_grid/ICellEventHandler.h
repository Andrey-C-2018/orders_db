#pragma once
#include "IGridCellWidget.h"

struct IOnCellChangedAction {
	virtual void executeAction() = 0;
	virtual ~IOnCellChangedAction() { }
};

struct ICellEventHandler {

	virtual void OnCellChanged(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) = 0;
	virtual void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
										IOnCellChangedAction &action) = 0;
	virtual ~ICellEventHandler() { }
};
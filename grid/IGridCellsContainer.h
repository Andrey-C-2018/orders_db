#pragma once
#include "IComposedDrawable.h"
#include "IInteractiveObject.h"

struct IGridCellsContainer : public IComposedDrawable, \
								public IInteractiveObject{

	virtual size_t GetVisibleRecordsCount(const int height) const = 0;
	virtual int FocusOnRecord(const size_t record_index, const int height) = 0;
	virtual int RemoveField(const size_t field, const int grid_widget_width, \
							const int sum) = 0;
	virtual ~IGridCellsContainer() { }
};

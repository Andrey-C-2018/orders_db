#pragma once
#include "IComposedDrawable.h"
#include "IInteractiveObject.h"

struct IGridCellsContainer : public IComposedDrawable, \
								public IInteractiveObject{

	virtual size_t GetVisibleRecordsCount(const int height) const = 0;
	virtual int FocusOnRecord(const size_t record_index, const int height) = 0;
	virtual ~IGridCellsContainer() { }
};

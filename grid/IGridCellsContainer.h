#pragma once
#include "IComposedDrawable.h"
#include "IInteractiveObject.h"
#include "IReloadable.h"

struct GridScrollParams {
	int offset;
	int records_sizes_summ;

	inline GridScrollParams() : offset(0), records_sizes_summ(0) { }
	inline GridScrollParams(const int offset_, \
							const int records_sizes_summ_) : \
							offset(offset_), records_sizes_summ(records_sizes_summ_) { }

	inline bool operator==(const GridScrollParams &obj) const {

		return offset == obj.offset && \
				records_sizes_summ == obj.records_sizes_summ;
	}
};

struct IGridCellsContainer : public IComposedDrawable, \
								public IInteractiveObject, \
								public IReloadable {

	virtual size_t GetVisibleRecordsCount(const int height) const = 0;
	virtual int FocusOnRecord(const size_t record_index, const int height) = 0;
	virtual int FocusOnField(const size_t field, const int grid_widget_width, \
							const int sum) = 0;
	virtual int RemoveField(const size_t field, const int grid_widget_width, \
							const int sum) = 0;
	virtual GridScrollParams GetVerticalScrollParams(const size_t new_items_count, \
														const int height) = 0;
	virtual ~IGridCellsContainer() { }
};

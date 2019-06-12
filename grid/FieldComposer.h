#pragma once
#include <assert.h>
#include <xwindows/XGC.h>
#include "IComposedDrawable.h"

class CFieldComposer {
	enum Defaults {
		DEF_RECORD_HEIGHT = 20
	};

	int record_height;
	int offset;
	size_t initial_item_index;
	int shift;

	IComposedDrawable *items;

	inline void RecalcInitialItemAndShift() {

		initial_item_index = offset / record_height;
		shift = offset % record_height;
	}

	CFieldComposer(const CFieldComposer &obj) = delete;
	CFieldComposer &operator=(const CFieldComposer &obj) = delete;
	CFieldComposer &operator=(CFieldComposer &&obj) = delete;
public:
	CFieldComposer(IComposedDrawable *items_) : \
					record_height(DEF_RECORD_HEIGHT), \
					offset(0), \
					initial_item_index(0), \
					shift(0), items(items_) {

		assert(items_);
	}

	CFieldComposer(IComposedDrawable *items_, CFieldComposer &&obj) : \
					record_height(obj.record_height), \
					offset(obj.offset), \
					initial_item_index(obj.initial_item_index), \
					shift(obj.shift), items(items_) {

		obj.Reset();
	}

	inline void SetRecordHeight(const int record_height) {

		assert(record_height);
		this->record_height = record_height;
		if (offset) RecalcInitialItemAndShift();
	}

	inline int Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) {
		XPoint item_coords(initial_coords);
		const XSize item_size(size.width, record_height);
		int y = -shift;

		size_t items_count = items->count();
		for (size_t i = initial_item_index; \
			i < items_count && y < size.height + record_height; \
			++i) {
			IDrawable &item = items->GetByIndex(i);

			item_coords.y = initial_coords.y + y;
			item.Draw(gc, item_coords, item_size);

			y += record_height;
		}

		return item_coords.y + item_size.height * (items_count != 0);
	}

	void OnScroll(const int scroll_pos) {

		this->offset = scroll_pos;
		RecalcInitialItemAndShift();
	}

	inline std::pair<int, int> GetCurrViewParams(const size_t new_items_count, \
														const int height) {

		int all_records_size = (int)new_items_count * record_height;
		if (offset && offset + height > all_records_size) {
			offset = height - (all_records_size - offset);
			RecalcInitialItemAndShift();
		}

		return std::pair<int, int>(offset, all_records_size);
	}

	size_t GetItemIndexByCoord(const int coord) { 
		size_t item_index = (size_t)((coord + shift) / record_height);
		
		item_index += initial_item_index;
		return item_index;
	}

	inline size_t GetVisibleItemsCount(const int height) const {

		return height / record_height;
	}

	int FocusOnItem(const size_t item_index, const int height) {

		assert(item_index < items->count());

		size_t first_visible_item = shift ? initial_item_index + 1: \
											initial_item_index;
		size_t last_item = first_visible_item + \
							(height - (record_height - shift) * (shift > 0)) \
							/ record_height;

		if (item_index >= first_visible_item && item_index < last_item) 
			return offset;

		if (item_index >= last_item) {
			size_t visible_items_count = GetVisibleItemsCount(height);
			int reminder = height % record_height;

			shift = record_height - reminder;
			initial_item_index = item_index - visible_items_count;
		}
		else {
			shift = 0;
			initial_item_index = item_index;
		}

		offset = (int)initial_item_index * record_height + shift;
		return offset;
	}

	inline void Reset() {

		record_height = 0;
		offset = 0;
		initial_item_index = 0;
		shift = 0;
		items = nullptr;
	}

	virtual ~CFieldComposer() { }
};

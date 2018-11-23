#pragma once
#include <memory>
#include "RecordEvaluator.h"
#include "ISizes.h"
#include "IComposedDrawable.h"
#include <xwindows\XGC.h>

class CRecordComposer {
	IComposedDrawable *items;
	
	struct ViewParams {
		std::shared_ptr<const ISizes> items_sizes;
		CRecordEvaluator rec_evaluator;
		CRecordEvaluator::CPosition view_params;
		int prev_offset;

		inline ViewParams(std::shared_ptr<const ISizes> items_sizes_) : \
							items_sizes(items_sizes_), \
							prev_offset(0){ }
	};
	std::shared_ptr<ViewParams> shv;
	std::shared_ptr<const ViewParams> shvc;

	mutable CRecordEvaluator onclick_evaluator;
	mutable int onclick_last_coord;

	class CSizesPredicate {
		std::shared_ptr<const ISizes> items_sizes;
	public:
		CSizesPredicate(std::shared_ptr<const ISizes> items_sizes_) : \
			items_sizes(items_sizes_) { }
		inline int operator()(const size_t index) const {

			return items_sizes->GetWidth(index);
		}
	};

public:
	enum {
		NULL_ITEM_INDEX = (size_t)-1
	};

	CRecordComposer(IComposedDrawable *items_, \
					std::shared_ptr<const ISizes> items_sizes_) : \
						items(items_), \
						shv(std::make_shared<ViewParams>(items_sizes_)), \
						shvc(shv), onclick_last_coord(0) {
				
		assert(items_);
		assert(items_sizes_);
		assert(items->count() == shvc->items_sizes->size());
	}

	CRecordComposer(IComposedDrawable *items_, \
					CRecordComposer &&obj) : items(items_), \
					shv(), \
					shvc(std::move(obj.shvc)), \
					onclick_evaluator(std::move(obj.onclick_evaluator)), \
					onclick_last_coord(obj.onclick_last_coord) {
	
		assert(items_);
		assert(items->count() == shvc->items_sizes->size());
		obj.onclick_last_coord = 0;
	}

	CRecordComposer(IComposedDrawable *items_, \
					const CRecordComposer &obj) : items(items_), \
					shv(), \
					shvc(obj.shvc), \
					onclick_evaluator(obj.onclick_evaluator), \
					onclick_last_coord(obj.onclick_last_coord) {
	
		assert(items_);
		assert(items->count() == shvc->items_sizes->size());
	}
	
	CRecordComposer(const CRecordComposer &obj) = default;
	CRecordComposer(CRecordComposer &&obj) = default;
	CRecordComposer &operator=(const CRecordComposer &obj) = default;
	CRecordComposer &operator=(CRecordComposer &&obj) = default;

	inline void Init(IComposedDrawable *items) {

		assert(items);
		assert(items->count() == shvc->items_sizes->size());
		this->items = items;
	}

	inline int Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) {

		assert(shvc);
		size_t count = items->count();
		int x = shvc->view_params.x - shvc->view_params.offset;
		
		XPoint item_coords(initial_coords);
		XSize item_size(size);

		size_t i = shvc->view_params.item_index;
		while (x < size.width && i < count) {

			item_coords.x = initial_coords.x + x;
			item_size.width = shvc->items_sizes->GetWidth(i);

			IDrawable &item = items->GetByIndex(i);
			item.Draw(gc, item_coords, item_size);

			x += item_size.width;
			++i;
		}

		return item_coords.x + item_size.width;
	}

	void OnScroll(const int scroll_pos) {

		assert(shv);
		assert(shvc);
		int offset = shv->view_params.offset = scroll_pos;

		size_t count = items->count();
		shv->view_params = shv->rec_evaluator.EvalPosition(offset >= shvc->prev_offset, \
												count, \
												CSizesPredicate(shvc->items_sizes), \
												offset);
		shv->prev_offset = offset;
	}

	size_t GetItemIndexByCoord(const int coord) const {

		assert(shvc);
		if (onclick_evaluator < shvc->rec_evaluator)
			onclick_evaluator = shvc->rec_evaluator;

		size_t count = items->count();
		int new_offset = shvc->view_params.offset + coord;
		auto onclick_view_params = onclick_evaluator.EvalPosition(new_offset >= onclick_last_coord, \
															count, \
															CSizesPredicate(shvc->items_sizes), \
															new_offset);
		onclick_last_coord = onclick_view_params.x;

		bool out_of_range = (onclick_view_params.item_index >= count);
		return !out_of_range * onclick_view_params.item_index + \
				out_of_range * NULL_ITEM_INDEX;
	}

	inline int RemoveField(const size_t field, const int grid_widget_width, \
							const int total_sum) {
		assert(shv);
		assert(shvc);

		shv->view_params = shv->rec_evaluator.RemoveItem(total_sum, grid_widget_width, \
										items->count(), field, \
										CSizesPredicate(shvc->items_sizes), \
										shvc->prev_offset);
		shv->prev_offset = shv->view_params.offset;

		return shv->view_params.offset;
	}

	inline void Reset() {

		items = nullptr;
		if (shv) {
			shv->view_params.offset = 0;
			shv->view_params.item_index = 0;
			shv->view_params.x = 0;

			shv->prev_offset = 0;
			shv->items_sizes = nullptr;
		}

		shv.reset();
		shvc.reset();

		onclick_last_coord = 0;
	}

	virtual ~CRecordComposer() { }
};

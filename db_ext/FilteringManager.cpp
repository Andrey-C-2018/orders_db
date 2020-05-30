#include <assert.h>
#include <db/IDbBindingTarget.h>
#include "FilteringManager.h"

CFilteringManager::CFilteringManager() : filtering_changed(false), \
										flt_string_changed(false), \
										flt_str_to_be_erased(false) { }

int CFilteringManager::addExpr(ImmutableString<char> expression, \
								std::shared_ptr<IBinder> binder) {

	assert(!expression.isNull());
	assert(binder);

	FilteringItem item;
	size_t size = filtering_items.size();
	item.id = size ? filtering_items[size - 1].id + 1 : 0;
	item.expression = expression.str;

	item.binders.push_back(binder);
	item.enabled = false;
	filtering_items.emplace_back(std::move(item));
		
	return item.id;
}

void CFilteringManager::addBinderToExpr(const int id_expr, \
										std::shared_ptr<IBinder> binder) {

	assert(binder);

	auto p = findFilteringItem(id_expr);
	p->binders.push_back(binder);
}

void CFilteringManager::enableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	if (!p->enabled) {
		enabled_items[id_expr] = std::distance(filtering_items.begin(), p);
		flt_string_changed = flt_string_changed || \
						flt_str_items.find(id_expr) == flt_str_items.cend();

		bool empty = flt_str_items.empty();
		flt_str_to_be_erased = flt_str_to_be_erased || \
							!empty && flt_str_items.rbegin()->first > id_expr;
	}

	filtering_changed = true;
	p->enabled = true;
}

void CFilteringManager::disableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	if (p->enabled)
		enabled_items.erase(id_expr);

	filtering_changed = true;
	p->enabled = false;
}

void CFilteringManager::disableAll() {

	for (auto i : enabled_items)
		filtering_items[i.second].enabled = false;

	filtering_changed = !enabled_items.empty();
	enabled_items.clear();
}

bool CFilteringManager::apply(std::shared_ptr<IDbBindingTarget> parsed_query) {

	if (!filtering_changed) return true;

	assert(parsed_query);

	size_t params_counter = 0;
	for(const auto &i : flt_str_items) {

		auto &flt_str_item = i.second;
		auto &item = filtering_items[flt_str_item.index];

		parsed_query->bindValue(params_counter, !item.enabled);
		++params_counter;

		if (item.enabled) {
			auto p_binder = item.binders.begin();
			while (p_binder != item.binders.end()) {
				if (!p_binder->get()->bind(parsed_query, params_counter))
					return false;
				++p_binder;
				++params_counter;
			}
		}
		else
			params_counter += item.binders.size();
	}

	filtering_changed = false;
	return true;
}

CFilteringManager::~CFilteringManager() { }

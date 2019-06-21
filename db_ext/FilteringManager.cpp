#include <assert.h>
#include <db/IDbBindingTarget.h>
#include "FilteringManager.h"

CFilteringManager::CFilteringManager() : query_text(), \
									filtering_changed(false), \
									query_text_changed(false), \
									switched_expr_count(0) { }

int CFilteringManager::addExpr(ImmutableString<char> expression, \
								std::shared_ptr<IBinder> binder) {

	assert(!expression.isNull());
	assert(binder);

	FilteringItem item;
	size_t size = filtering_items.size();
	item.id = size ? filtering_items[size - 1].id + 1 : 0;
	item.expression = expression.str;

	item.pos_begin = 0;
	item.pos_end = 0;
	item.enabled = false;
	item.switched = false;
	item.first_param = -1;

	item.binders.push_back(binder);
	filtering_items.emplace_back(std::move(item));
		
	return item.id;
}

void CFilteringManager::addBinderToExpr(const int id_expr, \
										std::shared_ptr<IBinder> binder) {

	assert(binder);

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	p->binders.push_back(binder);
}

void CFilteringManager::enableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	InternalEnable(p);
	filtering_changed = true;
}

void CFilteringManager::enableAll() {

	for (auto p = filtering_items.begin(); p != filtering_items.end(); ++p) 
		InternalEnable(p);

	filtering_changed = true;
}

void CFilteringManager::disableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	InternalDisable(p);
	filtering_changed = true;
}

void CFilteringManager::disableAll() {

	for (auto p = filtering_items.begin(); p != filtering_items.end(); ++p)
		InternalDisable(p);

	filtering_changed = true;
}

void CFilteringManager::apply(std::shared_ptr<IDbBindingTarget> parsed_query) {

	assert(filtering_changed || (!filtering_changed && !query_text_changed));
	if (!filtering_changed) return;

	assert(parsed_query);

	int params_counter = 0;
	for(auto p = filtering_items.begin(); p != filtering_items.end(); ++p) {

		p->first_param = params_counter;
		if (p->switched) {
			parsed_query->bindValue(p->first_param, !p->enabled);
			++params_counter;
		}

		if(p->enabled)
			for(auto p_binder = p->binders.begin(); \
				p_binder != p->binders.end(); ++p_binder, ++params_counter)
					p_binder->get()->bind(parsed_query, params_counter);

	}

	filtering_changed = false;
	query_text_changed = false;
}

CFilteringManager::~CFilteringManager() { }

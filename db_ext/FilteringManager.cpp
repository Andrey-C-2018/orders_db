#include <assert.h>
#include <db/IDbBindingTarget.h>
#include "FilteringManager.h"

CFilteringManager::CFilteringManager(ImmutableString<char> query_text_) : \
									query_text(query_text_.str), \
									filtering_changed(false) { }

int CFilteringManager::addExpr(ImmutableString<char> expression, \
								std::unique_ptr<IBinder> binder) {

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

	item.binders.emplace_back(std::move(binder));
	filtering_items.emplace_back(item);
		
	return item.id;
}

void CFilteringManager::addBinderToExpr(const int id_expr, std::unique_ptr<IBinder> binder) {

	assert(binder);

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	p->binders.emplace_back(std::move(binder));
}

void CFilteringManager::enableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	bool prev_switched = p->switched;
	if(!p->switched && switched_expr_count < MAX_SWITCHED_EXPR_COUNT) {
		
		p->expression.insert(0, "? OR (");
		p->expression += ')';

		p->switched = true;
		++switched_expr_count;
	}
	
	if (!isExprAddedToQuery(*p) && \
		(prev_switched != p->switched || (!p->switched && !p->enabled))) {
	
		auto pos = addExprToWhereStmt(query_text, p->expression);
		p->pos_begin = pos.first;
		p->pos_end = pos.second;
	}

	p->enabled = true;
	filtering_changed = true;
}

void CFilteringManager::disableExpr(int id_expr) {

	auto p = findFilteringItem(id_expr);
	assert(isFound(p, id_expr));

	if (isExprAddedToQuery(*p) && !p->switched && p->enabled) {

		query_text.erase(p->pos_begin, p->pos_end - p->pos_begin);
		p->pos_begin = 0;
		p->pos_end = 0;
	}

	p->enabled = false;
	filtering_changed = true;
}

void CFilteringManager::apply(std::shared_ptr<IDbBindingTarget> parsed_query) {

	if (!filtering_changed) return;

	assert(parsed_query);

	int params_counter = 0;
	for(auto p = filtering_items.begin(); p != filtering_items.end(); ++p) {

		p->first_param = params_counter;
		if (p->switched) {
			parsed_query->bindValue(p->first_param, p->enabled);
			++params_counter;
		}

		if(p->enabled)
			for(auto p_binder = p->binders.begin(); \
				p_binder != p->binders.end(); ++p_binder, ++params_counter)
					p_binder->get()->bind(parsed_query, params_counter);

	}

	filtering_changed = false;
}

CFilteringManager::~CFilteringManager() { }

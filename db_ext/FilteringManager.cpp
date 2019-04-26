#include "FilteringManager.h"

CFilteringManager::CFilteringManager(std::shared_ptr<CDbTable> db_table_, \
										ImmutableString<char> query_text) : \
							db_table(db_table_), query_modifier(query_text) { }

void CFilteringManager::setDefaultFilterExpr(ImmutableString<char> expression) {

}

int CFilteringManager::addExpr(ImmutableString<char> expression, \
				std::unique_ptr<IBinder> binder) {

	if(!disabled_exprs_count){
		
		query_modifier.addExprToWhereStmt(expression);

		FilteringItem item;
		item.expression = expression.str;
		item.index = query_modifier.getExprCount() - 1;
		item.disabled = false;

		filtering_items.push_back(item);
		item.binders.emplace_back(binder);
	}
	else {
		
		query_modifier.setExpr(disabled_exprs_count - 1, expression);
		--disabled_exprs_count;
	}

	db_table.setQuery(query_modifier.getQuery());
}

void CFilteringManager::addBinderToExpr(const int id_expr, std::unique_ptr<IBinder> binder) {

}

void CFilteringManager::enableExpr(int id_expr) {

}

void CFilteringManager::disableExpr(int id_expr) {

}

void CFilteringManager::resetFilterToDefault() {

}

void CFilteringManager::setDefaultOrderingValue(ImmutableString<char> expression) {

}

void CFilteringManager::setOrderingField(const bool asc) {

}

void CFilteringManager::resetOrderingToDefault() {

}

void CFilteringManager::resetAllToDefault() {

}

void CFilteringManager::apply() {

}

CFilteringManager::~CFilteringManager() {

}

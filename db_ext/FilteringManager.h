#include <vector>
#include <memory>
#include <algorithm>
#include <basic/ImmutableString.h>
#include <db_ext/IBinder.h>

class CFilteringManager {
	enum {
		MAX_SWITCHED_EXPR_COUNT = 5
	};

	std::string query_text;
	bool filtering_changed;
	size_t switched_expr_count;

	struct FilteringItem {
		int id;
		std::string expression;
		size_t pos_begin, pos_end;
		bool enabled;
		bool switched;
		std::vector<std::shared_ptr<IBinder> > binders;
		int first_param;
	};

	class CSearchPredicate {
		const std::vector<FilteringItem> &filtering_items;
	public:
		CSearchPredicate(const std::vector<FilteringItem> &filtering_items_) : \
							filtering_items(filtering_items_) { }
		inline bool operator()(const int id_expr, const FilteringItem &item) {

			return id_expr < item.id;
		}

		inline bool operator()(const FilteringItem &item, const int id_expr) {

			return item.id < id_expr;
		}
	};

	std::vector<FilteringItem> filtering_items;
	
	inline auto findFilteringItem(const int id_expr) const;
	inline auto findFilteringItem(const int id_expr);
	inline bool isFound(std::vector<FilteringItem>::const_iterator, \
						const int id_expr) const;
	inline bool isFound(std::vector<FilteringItem>::iterator, \
						const int id_expr) const;
	inline static std::pair<size_t, size_t> \
			addExprToWhereStmt(std::string &text, \
								const std::string &expression);
	inline bool isExprAddedToQuery(const FilteringItem &item) const;

	inline void InternalEnable(std::vector<FilteringItem>::iterator p);
	inline void InternalDisable(std::vector<FilteringItem>::iterator p);
public:
	CFilteringManager();
	CFilteringManager(const CFilteringManager &obj) = default;
	CFilteringManager(CFilteringManager &&obj) = default;
	CFilteringManager &operator=(const CFilteringManager &obj) = default;
	CFilteringManager &operator=(CFilteringManager &&obj) = default;

	int addExpr(ImmutableString<char> expression, \
				std::shared_ptr<IBinder> binder);
	void addBinderToExpr(const int id_expr, std::shared_ptr<IBinder> binder);

	void enableExpr(int id_expr);
	void enableAll();
	void disableExpr(int id_expr);
	void disableAll();
	
	inline ImmutableString<char> getModifiedQuery() const;
	void apply(std::shared_ptr<IDbBindingTarget> parsed_query);

	virtual ~CFilteringManager();
};

//*****************************************************

auto CFilteringManager::findFilteringItem(const int id_expr) const {

	return std::lower_bound(filtering_items.cbegin(), filtering_items.cend(), \
							id_expr, CSearchPredicate(filtering_items));
}

auto CFilteringManager::findFilteringItem(const int id_expr) {

	return std::lower_bound(filtering_items.begin(), filtering_items.end(), \
							id_expr, CSearchPredicate(filtering_items));
}

bool CFilteringManager::isFound(std::vector<FilteringItem>::const_iterator p, \
								const int id_expr) const {

	return p != filtering_items.cend() && p->id == id_expr;
}

bool CFilteringManager::isFound(std::vector<FilteringItem>::iterator p, \
								const int id_expr) const {

	return p != filtering_items.end() && p->id == id_expr;
}

std::pair<size_t, size_t> \
		CFilteringManager::addExprToWhereStmt(std::string &text, \
												const std::string &expression) {
	std::pair<size_t, size_t> pos;
	pos.first = text.size();

	if(pos.first) text += " AND ";
	text += '(';
	text += expression;
	text += ')';
	pos.second = text.size();

	return pos;
}

bool CFilteringManager::isExprAddedToQuery(const FilteringItem &item) const {

	return item.pos_end && item.pos_begin != item.pos_end;
}

ImmutableString<char> CFilteringManager::getModifiedQuery() const {

	return ImmutableString<char>(query_text.c_str(), query_text.size());
}

void CFilteringManager::InternalEnable(std::vector<FilteringItem>::iterator p) {

	bool prev_switched = p->switched;
	if (!p->switched && switched_expr_count < MAX_SWITCHED_EXPR_COUNT) {
		const char str_switch[] = "? OR (";

		if (p->binders.size() > 1) {
			p->expression.insert(0, str_switch);
			p->expression += ')';
		}
		else
			p->expression.insert(0, str_switch, sizeof(str_switch) - 2);

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
}

void CFilteringManager::InternalDisable(std::vector<FilteringItem>::iterator p) {

	if (isExprAddedToQuery(*p) && !p->switched && p->enabled) {

		query_text.erase(p->pos_begin, p->pos_end - p->pos_begin);
		p->pos_begin = 0;
		p->pos_end = 0;
	}

	p->enabled = false;
}
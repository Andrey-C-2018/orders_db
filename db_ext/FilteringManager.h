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

	struct FilteringItem {
		int id;
		std::string expression;
		size_t pos_begin, pos_end;
		bool enabled;
		bool switched;
		std::vector<std::unique_ptr<IBinder> > binders;
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
	size_t switched_expr_count;

	auto findFilteringItem(const int id_expr) const;
	auto findFilteringItem(const int id_expr);
	inline bool isFound(std::vector<FilteringItem>::const_iterator, \
						const int id_expr) const;
	inline bool isFound(std::vector<FilteringItem>::iterator, \
						const int id_expr) const;
	inline static std::pair<size_t, size_t> \
			addExprToWhereStmt(std::string &text, \
								const std::string &expression);
	inline bool isExprAddedToQuery(const FilteringItem &item) const;

public:
	CFilteringManager(ImmutableString<char> query_text);

	int addExpr(ImmutableString<char> expression, std::unique_ptr<IBinder> binder);
	void addBinderToExpr(const int id_expr, std::unique_ptr<IBinder> binder);

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

	text += ", ";
	text += expression;
	pos.second = text.size();

	return pos;
}

bool CFilteringManager::isExprAddedToQuery(const FilteringItem &item) const {

	return item.pos_end && item.pos_begin != item.pos_end;
}

ImmutableString<char> CFilteringManager::getModifiedQuery() const {

	return ImmutableString<char>(query_text.c_str(), query_text.size());
}

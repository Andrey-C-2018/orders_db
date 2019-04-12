#include <db_ext/IBinder.h>

class CQueryModifier final {
	ImmutableString<char> query;
	size_t expr_count;
	bool no_where_stmt, ordering_enabled;
public:
	CQueryModifier(ImmutableString<char> query);

	inline size_t getExprCount() const;
	void addExprToWhereStmt(ImmutableString<char> expr);
	void setExpr(const size_t expr_index, ImmutableString<char> expr);
	void removeExprFromWhereStmt(const size_t expr_index);
	void setWhereStmt(ImmutableString<char> where_stmt);
	
	void setOrdering(const size_t field);
	void setOrdering(ImmutableString<char> expr);
	void removeOrdering();
};

class CFilteringManager {
	enum {
		DISABLED_EXPR_LIMIT = 5
	};

	std::shared_ptr<CDbTable> db_table;
	CQueryModifier query_modifier; 

	struct FilteringItem {
		std::string expression;
		size_t index;
		bool disabled;
		std::vector<std::unique_ptr<IBinder> > binders;
	};

	std::vector<FilteringItem> filtering_items;
	size_t disabled_exprs[DISABLED_EXPR_LIMIT];
	size_t disabled_exprs_count;

	size_t field_to_order;

	std::string def_filter_expr;
	std::string def_ordering;
public:
	CFilteringManager(std::shared_ptr<CDbTable> db_table_, \
				ImmutableString<char> query_text);

	void setDefaultFilterExpr(ImmutableString<char> expression);
	int addExpr(ImmutableString<char> expression, std::unique_ptr<IBinder> binder);
	void addBinderToExpr(const int id_expr, std::unique_ptr<IBinder> binder);

	void enableExpr(int id_expr);
	void disableExpr(int id_expr);
	void resetFilterToDefault();

	void setDefaultOrderingValue(ImmutableString expression);
	void setOrderingField(const bool asc);
	void resetOrderingToDefault();

	void resetAllToDefault();
	void apply();

	virtual ~CFilteringManager();
};

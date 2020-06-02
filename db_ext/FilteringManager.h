#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <basic/ImmutableString.h>
#include <basic/Exception.h>
#include <db_ext/IBinder.h>

class CFilteringManager {

	struct FilteringItem {
		int id;
		std::string expression;
		std::vector<std::shared_ptr<IBinder> > binders;
		size_t count_to_process;
		bool enabled;

		inline FilteringItem() noexcept : id(-1) { }
		inline FilteringItem(const int id_) noexcept : id(id_) { }

		inline bool operator<(const FilteringItem &obj) const {

			return id < obj.id;
		}
	};

	struct FltStringItem {
		size_t index;
		size_t pos_begin, pos_end;
	};

	std::string flt_string;
	bool filtering_changed, flt_string_changed;
	bool flt_str_to_be_erased;

	std::vector<FilteringItem> filtering_items;
	std::map<int, size_t> enabled_items;
	std::map<int, FltStringItem> flt_str_items;

	inline auto findFilteringItem(const int id);
	
public:
	CFilteringManager();

	CFilteringManager(const CFilteringManager &obj) = default;
	CFilteringManager(CFilteringManager &&obj) = default;
	CFilteringManager &operator=(const CFilteringManager &obj) = default;
	CFilteringManager &operator=(CFilteringManager &&obj) = default;

	int addExpr(ImmutableString<char> expression, \
				std::shared_ptr<IBinder> binder);
	template <size_t str_size> \
		inline int addExpr(const char (&str)[str_size], \
							std::shared_ptr<IBinder> binder);
	void addBinderToExpr(const int id_expr, std::shared_ptr<IBinder> binder);

	void enableExpr(int id_expr);
	void disableExpr(int id_expr);
	void disableAll();
	
	inline ImmutableString<char> buildFilteringString();
	inline bool isFilteringStringChanged() const;
	bool apply(std::shared_ptr<IDbBindingTarget> parsed_query);

	virtual ~CFilteringManager();
};

//*****************************************************

auto CFilteringManager::findFilteringItem(const int id) {

	auto p = std::lower_bound(filtering_items.begin(), filtering_items.end(), \
								FilteringItem(id));
	if (!(p != filtering_items.end() && p->id == id)) {
		XException e(0, _T("this filtering expression ID is not found: "));
		e << id;
		throw e;
	}

	return p;
}

ImmutableString<char> CFilteringManager::buildFilteringString() {

	if(flt_str_to_be_erased) {
		flt_str_items.clear();
		flt_string.clear();
		flt_str_to_be_erased = false;
	}
	
	for (auto i : enabled_items) {
		auto &item = filtering_items[i.second];

		auto p_flt = flt_str_items.find(item.id);
		if (p_flt == flt_str_items.cend()) {
			FltStringItem str_item;
			str_item.index = i.second;
			str_item.pos_begin = flt_string.size();

			if (!flt_string.empty()) flt_string += " AND ";
			flt_string += "(? OR (";
			flt_string += item.expression;
			flt_string += "))";

			str_item.pos_end = flt_string.size();
			flt_str_items.insert(\
				std::pair<int, FltStringItem>(item.id, str_item));
		}
	}

	flt_string_changed = false;
	return ImmutableString<char>(flt_string.c_str(), flt_string.size());
}

bool CFilteringManager::isFilteringStringChanged() const {

	return flt_string_changed;
}

template<size_t str_size> \
int CFilteringManager::addExpr(const char(&str)[str_size], \
								std::shared_ptr<IBinder> binder) {
	
	return addExpr(ImmutableString<char>(str, str_size), binder);
}

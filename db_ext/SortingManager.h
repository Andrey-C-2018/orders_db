#pragma once
#include <vector>
#include <string>
#include <basic/ImmutableString.h>

class CSortingManager {
	const size_t columns_count;
	std::vector<size_t> sorting;
	mutable std::string buffer;
	bool is_changed;

public:
	CSortingManager(const size_t columns_count_);

	CSortingManager(const CSortingManager &obj) = default;
	CSortingManager(CSortingManager &&obj) = default;
	CSortingManager &operator=(const CSortingManager &obj) = default;
	CSortingManager &operator=(CSortingManager &&obj) = default;

	inline bool isOrderingChanged() const { return is_changed; }
	inline bool empty() { return sorting.empty(); }
	void sortByColumn(const size_t column_index);
	void addSortingByColumn(const size_t column_index);
	void reset();
	ImmutableString<char> buildOrderingString();

	virtual ~CSortingManager();
};

#pragma once
#include <vector>
#include <string>
#include <basic/ImmutableString.h>

class CSortingManager {
	const size_t columns_count;
	std::vector<size_t> sorting;
	mutable std::string buffer;

public:
	CSortingManager(const size_t columns_count_);

	CSortingManager(const CSortingManager &obj) = default;
	CSortingManager(CSortingManager &&obj) = default;
	CSortingManager &operator=(const CSortingManager &obj) = default;
	CSortingManager &operator=(CSortingManager &&obj) = default;

	void sortByColumn(const size_t column_index);
	void addSortingByColumn(const size_t column_index);
	void reset();
	ImmutableString<char> getOrderingString() const;

	virtual ~CSortingManager();
};

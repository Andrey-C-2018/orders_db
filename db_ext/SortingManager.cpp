#include <assert.h>
#include <basic/XConv.h>
#include "SortingManager.h"

CSortingManager::CSortingManager(const size_t columns_count_) :
								columns_count(columns_count_), \
								is_changed(false) { }

void CSortingManager::sortByColumn(const size_t column_index) {

	assert(column_index < columns_count);

	bool is_empty = sorting.empty();
	is_changed = is_empty || (!is_empty && sorting[0] != column_index);

	sorting.clear();
	sorting.push_back(column_index);
}

void CSortingManager::addSortingByColumn(const size_t column_index) {

	assert(column_index < columns_count);
	sorting.push_back(column_index);
	is_changed = true;
}

void CSortingManager::reset() {

	is_changed = !sorting.empty();
	sorting.clear();
}

ImmutableString<char> CSortingManager::buildOrderingString() {
	
	if (sorting.empty()) return ImmutableString<char>(nullptr, 0);

	char conv_buffer[getDigitsCountOfType<size_t>() + 1];

	buffer.clear();
	for (auto p = sorting.cbegin(); p != sorting.cend(); ++p) {
		buffer += XConv::ToString(*p, conv_buffer);
		buffer += ',';
	}

	buffer.erase(buffer.size() - 1);
	is_changed = false;

	return ImmutableString<char>(&buffer[0], buffer.size());
}

CSortingManager::~CSortingManager() { }

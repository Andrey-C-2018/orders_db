#include <assert.h>
#include <basic/XConv.h>
#include "SortingManager.h"

CSortingManager::CSortingManager(const size_t columns_count_) :
								columns_count(columns_count_) { }

void CSortingManager::sortByColumn(const size_t column_index) {

	sorting.clear();
	addSortingByColumn(column_index);
}

void CSortingManager::addSortingByColumn(const size_t column_index) {

	assert(column_index < columns_count);
	sorting.push_back(column_index);
}

void CSortingManager::reset() {

	sorting.clear();
}

ImmutableString<char> CSortingManager::getOrderingString() const {
	
	if (sorting.empty()) return ImmutableString<char>(nullptr, 0);

	char conv_buffer[getDigitsCountOfType<size_t>() + 1];

	buffer.clear();
	for (auto p = sorting.cbegin(); p != sorting.cend(); ++p) {
		buffer += XConv::ToString(*p, conv_buffer);
		buffer += ',';
	}

	buffer.erase(buffer.size() - 1);
	return ImmutableString<char>(&buffer[0], buffer.size());
}

CSortingManager::~CSortingManager() { }

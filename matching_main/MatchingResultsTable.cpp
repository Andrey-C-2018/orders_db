#include <algorithm>
#include <assert.h>
#include "MatchingResultsTable.h"

CMatchingResultsRecord::CMatchingResultsRecord(std::shared_ptr<const IRecord> rec1_, \
						const size_t rec1_size, \
						std::shared_ptr<const IRecord> rec2_, \
						const size_t rec2_size) : \
						rec1(rec1_), size1(rec1_size), \
						rec2(rec2_), size2(rec2_size){ }

size_t CMatchingResultsRecord::size() const{
	
return size1 + size2 + 1;
}

const char *CMatchingResultsRecord::getColValueAsCharArray(const size_t index) const{

assert(index < size1 + size2 + 1);
assert((rec1 && rec1->size() == size1) || !rec1);
assert((rec2 && rec2->size() == size2) || !rec2);

if(index < size1) return rec1 ? rec1->getColValueAsCharArray(index) : "";
if(index == size1) return " <-> ";
return rec2 ? rec2->getColValueAsCharArray(index - size1 - 1) : "";
}

//**************************************************************

CMatchingResultsTable::CMatchingResultsTable() : in1(nullptr), in2(nullptr), \
													size1(0), size2(0){ }

CMatchingResultsTable::CMatchingResultsTable(const int part, \
										std::shared_ptr<ITable> in, \
										const size_t index_begin, \
										const size_t index_end, \
										bool include_header) : in1(nullptr), \
																in2(nullptr){

assert(in);
if (part == LEFT_PART) {
	in1 = in;
	size1 = in1->getColumnsCount();
	size2 = 0;
}
if (part == RIGHT_PART) {
	in2 = in;
	size1 = 0;
	size2 = in2->getColumnsCount();
}

size_t rec_count = in->getRecordsCount();
assert(index_begin < rec_count);
assert(index_end <= rec_count);
assert(index_begin <= index_end);

if (include_header) initColNames();

indexes.reserve(index_end - index_begin);
if(part == LEFT_PART)
	for (size_t i = index_begin; i < index_end; ++i)
		addIndexes(i, NULL_INDEX);
else
	for (size_t i = index_begin; i < index_end; ++i)
		addIndexes(NULL_INDEX, i);

}

CMatchingResultsTable::CMatchingResultsTable(std::shared_ptr<ITable> in1_, \
										std::shared_ptr<ITable> in2_, \
										bool include_header) : \
										in1(in1_), in2(in2_){

assert(in1_);
assert(in2_);

size1 = in1 ? in1->getColumnsCount() : 0;
size2 = in2 ? in2->getColumnsCount() : 0;

if (include_header) initColNames();
}

size_t CMatchingResultsTable::getColumnsCount() const{
	
return size1 + size2 + 1;
}

size_t CMatchingResultsTable::getRecordsCount() const{
	
return indexes.size();
}

void CMatchingResultsTable::addIndexes(const size_t in1_rec_index, \
									const size_t in2_rec_index){
CMatch match;
			
assert(in1_rec_index != NULL_INDEX || in2_rec_index != NULL_INDEX);
match.in1_rec_index = in1_rec_index;
match.in2_rec_index = in2_rec_index;
indexes.push_back(match);
}

CMatchingResultsRecord CMatchingResultsTable::getRecord(const size_t index) const{

assert(index < getRecordsCount());

size_t idx1 = indexes[index].in1_rec_index;
size_t idx2 = indexes[index].in2_rec_index;
std::shared_ptr<const IRecord> rec1 = !in1 || idx1 == NULL_INDEX ? \
										nullptr : in1->getRecord(idx1);
std::shared_ptr<const IRecord> rec2 = !in2 || idx2 == NULL_INDEX ? \
										nullptr : in2->getRecord(idx2);

return CMatchingResultsRecord(rec1, size1, rec2, size2);
}

size_t CMatchingResultsTable::getColIndexByName(const char *col_name) const{
	
assert(col_name);
auto p = std::find(col_names.cbegin(), col_names.cend(), col_name);

return p != col_names.cend() ? \
		std::distance(col_names.cbegin(), p) : ITable::NOT_FOUND;
}

const char *CMatchingResultsTable::getColName(const size_t index) const{
	
assert(index < getColumnsCount());
return col_names[index].c_str();
}
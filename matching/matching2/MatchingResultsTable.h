#pragma once
#include <memory>
#include <vector>
#include <ostream>
#include "ITable.h"

class CMatchingResultsRecord : public IRecord{
	std::shared_ptr<const IRecord> rec1, rec2;
	size_t size1, size2;
public:
	CMatchingResultsRecord(std::shared_ptr<const IRecord> rec1_, \
						const size_t rec1_size, \
						std::shared_ptr<const IRecord> rec2_, \
						const size_t rec2_size);

	CMatchingResultsRecord(const CMatchingResultsRecord &obj) = default;
	CMatchingResultsRecord(CMatchingResultsRecord &&obj) = default;
	CMatchingResultsRecord &operator=(const CMatchingResultsRecord &obj) = default;
	CMatchingResultsRecord &operator=(CMatchingResultsRecord &&obj) = default;
	
	size_t size() const override;
	const char *getColValueAsCharArray(const size_t index) const override;
	
	~CMatchingResultsRecord(){ }
};

class CMatchingResultsTable : public IColumnsArray{
	std::shared_ptr<ITable> in1, in2;
	size_t size1, size2;
	
	struct CMatch{
		size_t in1_rec_index;
		size_t in2_rec_index;
	};
	std::vector<CMatch> indexes;

	std::vector<std::string> col_names;
	inline void initColNames();
public:
	enum{
		NULL_INDEX = (size_t)-1
	};
	enum Parts{
		LEFT_PART = 0, \
		RIGHT_PART = 1
	};

	CMatchingResultsTable();
	CMatchingResultsTable(const int part, std::shared_ptr<ITable> in, \
						const size_t index_begin, \
						const size_t index_end, \
						bool include_header = true);
	CMatchingResultsTable(std::shared_ptr<ITable> in1_, std::shared_ptr<ITable> in2_,\
						bool include_header = true);
	CMatchingResultsTable(const CMatchingResultsTable &obj) = default;
	CMatchingResultsTable(CMatchingResultsTable &&obj) = default;
	CMatchingResultsTable &operator=(const CMatchingResultsTable &obj) = default;
	CMatchingResultsTable &operator=(CMatchingResultsTable &&obj) = default;
	
	size_t getColumnsCount() const;
	size_t getRecordsCount() const;
	void addIndexes(const size_t in1_rec_index, const size_t in2_rec_index);
	CMatchingResultsRecord getRecord(const size_t index) const;
	size_t getColIndexByName(const char *col_name) const override;
	const char *getColName(const size_t index) const override;

	~CMatchingResultsTable(){ }
};

//**************************************************************

void CMatchingResultsTable::initColNames() {

col_names.reserve(size1 + size2 + 1);

for (size_t i = 0; i < size1; ++i)
	col_names.push_back(in1->getColName(i));

col_names.push_back(" <=> ");

for (size_t i = 0; i < size2; ++i)
	col_names.push_back(in2->getColName(i));
}


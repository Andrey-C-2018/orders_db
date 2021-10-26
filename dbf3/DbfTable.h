#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include <cassert>
#include <basic/Exception.h>
#include "DbfStructs.h"

class DbfTableException : public XException {
public:
	enum {
		E_PATH_IS_NULL = 1, \
		E_NO_DBF_FILE = 2, \
		E_VERSION = 3, \
		E_WRONG_FORMAT = 4
	};

	DbfTableException(int err_code, const char *err_descr);

	DbfTableException(const DbfTableException &obj);
	DbfTableException(DbfTableException &&obj) = default;

	virtual ~DbfTableException();
};

class DbfTable {
	std::ifstream f;
	std::unique_ptr<DBaseHeader> header;
	std::vector<DBaseField> fields;

	bool rec_remove_mark;
	std::vector<std::vector<char> > field_buffers;
	size_t curr_rec;

	void readHeader(const char *table_path);
	void initFields();

public:
	DbfTable();
	explicit DbfTable(const char *table_path);

	void open(const char *table_path);
	
	DbfTable(const DbfTable &obj) = delete;
	DbfTable(DbfTable &&obj) = default;
	DbfTable &operator=(const DbfTable &obj) = delete;
	DbfTable &operator=(DbfTable &&obj) = default;

	inline bool isOpen() const { return f.is_open(); }

	inline size_t getFieldsCount() const;
	inline const char *getFieldName(const size_t field) const;
	inline char getFieldType(const size_t field) const;
	inline size_t getRecordsCount() const;

	inline void gotoFirstRecord();
	bool fetchNext();
	inline size_t getCurrentRec() const;

	inline bool removeMark() const;
	inline const char *getStrCellValue(const size_t field) const;
	inline const char *getNumericCellValue(const size_t field) const;
	inline const char *getDateCellValue(const size_t field) const;
	inline bool getBooleanCellValue(const size_t field) const;

	void close();
	virtual ~DbfTable();
};

//****************************************************

size_t DbfTable::getFieldsCount() const {
	
	return fields.size();
}

const char *DbfTable::getFieldName(const size_t field) const {
	
	assert(field < fields.size());
	return fields[field].name;
}

char DbfTable::getFieldType(const size_t field) const {
	
	assert(field < fields.size());
	return fields[field].type;
}

size_t DbfTable::getRecordsCount() const {

	assert(header);
	return header->recs_count;
}

void DbfTable::gotoFirstRecord() {
	
	assert(header);
	f.seekg(header->length);
	curr_rec = (size_t)-1;
}

size_t DbfTable::getCurrentRec() const {

	return curr_rec;
}

const char *DbfTable::getStrCellValue(const size_t field) const {
	
	assert(curr_rec != (size_t)-1);
	assert(field < fields.size());
	return &field_buffers[field][0];
}

const char *DbfTable::getNumericCellValue(const size_t field) const {

	assert(fields[field].type == DBF_FIELD_NUM);
	return getStrCellValue(field);
}

const char *DbfTable::getDateCellValue(const size_t field) const {

	assert(fields[field].type == DBF_FIELD_DATE);
	return getStrCellValue(field);
}

bool DbfTable::getBooleanCellValue(const size_t field) const {

	assert(curr_rec != (size_t)-1);
	assert(field < fields.size());
	assert(fields[field].type == DBF_FIELD_LOGIC);
	return field_buffers[field][0] == 'T';
}

bool DbfTable::removeMark() const {

	assert(curr_rec != (size_t)-1);
	return rec_remove_mark;
}

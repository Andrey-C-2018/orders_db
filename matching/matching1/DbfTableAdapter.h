#pragma once
#include <cassert>
#include <basic/Exception.h>
#include <dbf3/DbfTable.h>

class CDbfTableAdapter {
	DbfTable dbf_table;
	std::string dbf_dir;

public:
	enum ColumnsTypesSizes {
		INT_SIZE = 4, 
		DATE_SIZE = 10
	};

	CDbfTableAdapter();
	inline void setDbfDir(const char *dbf_dir);
	void open(const char *dbf_path);
	void openByName(const char *dbf_file_name);
	
	CDbfTableAdapter(const CDbfTableAdapter &obj) = delete;
	CDbfTableAdapter(CDbfTableAdapter &&obj) = delete;
	CDbfTableAdapter &operator=(const CDbfTableAdapter &obj) = delete;
	CDbfTableAdapter &operator=(CDbfTableAdapter &&obj) = delete;

	inline size_t getFieldsCount() const;
	inline const char *getFieldName(size_t field) const;
	inline const wchar_t *getFieldNameW(size_t field) const;

	inline bool gotoFirstRecord();
	inline bool gotoNextRecord();

	const char *getCStringCellValue(size_t field);
	const wchar_t *getCWStringCellValue(size_t field);
	int getIntCellValue(size_t field, bool &is_null);
	const char *getNumericCellValue(size_t field);
	const char *getDateCellValue(size_t field, char *buffer);

	void close();
	virtual ~CDbfTableAdapter();
};

//****************************************************

void CDbfTableAdapter::setDbfDir(const char *dbf_dir_) {

	assert(dbf_dir_);
	this->dbf_dir = dbf_dir_;
}

size_t CDbfTableAdapter::getFieldsCount() const {

	return dbf_table.getFieldsCount();
}

const char *CDbfTableAdapter::getFieldName(const size_t field) const {

	return dbf_table.getFieldName(field);
}

const wchar_t *CDbfTableAdapter::getFieldNameW(size_t field) const {

	return nullptr;
}

bool CDbfTableAdapter::gotoFirstRecord() {

	if(!dbf_table.getFieldsCount()) return false;

	dbf_table.gotoFirstRecord();
	return dbf_table.fetchNext();
}

bool CDbfTableAdapter::gotoNextRecord() {

	return dbf_table.fetchNext();
}

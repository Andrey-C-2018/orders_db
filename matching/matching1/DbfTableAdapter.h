#pragma once
#include <cassert>
#include <basic/Exception.h>
#include <basic/Path.h>
#include <dbf3/DbfTable.h>

class CDbfTableAdapter {
	DbfTable dbf_table;
	Tstring dbf_file_path;
	size_t dbf_dir_path_len;

	void open(const char *dbf_path);
	void open(const wchar_t *dbf_path);

public:
	enum ColumnsTypesSizes {
		INT_SIZE = 4, 
		DATE_SIZE = 10
	};

	CDbfTableAdapter();
	
	CDbfTableAdapter(const CDbfTableAdapter &obj) = delete;
	CDbfTableAdapter(CDbfTableAdapter &&obj) = delete;
	CDbfTableAdapter &operator=(const CDbfTableAdapter &obj) = delete;
	CDbfTableAdapter &operator=(CDbfTableAdapter &&obj) = delete;

	inline Tstring getDbfDir() const;
	inline void setDbfDir(const Tchar *dbf_dir);
	inline void setDbfFileName(const Tchar *dbf_file_name);
	inline void open();
	inline const Tchar *getDbfFilePath() const;

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

Tstring CDbfTableAdapter::getDbfDir() const {

	// TO DO
}

void CDbfTableAdapter::setDbfDir(const Tchar *dbf_dir_) {

	assert(dbf_dir_ && dbf_dir_[0] != _T('\0'));

	Tstring file_name;
	if(dbf_dir_path_len != dbf_file_path.size())
		file_name.assign(dbf_file_path, dbf_dir_path_len);

	dbf_file_path = dbf_dir_;

	if(dbf_file_path.back() != PATH_SLASH)
		dbf_file_path += PATH_SLASH;
	dbf_dir_path_len = dbf_file_path.size();

	dbf_file_path += file_name;
}

void CDbfTableAdapter::setDbfFileName(const Tchar *dbf_file_name) {

	assert(dbf_dir_path_len > 0);
	assert(dbf_file_name && dbf_file_name[0] != _T('\0'));

	dbf_file_path.erase(dbf_dir_path_len + 1, dbf_file_path.size() - dbf_dir_path_len - 1);
	dbf_file_path += dbf_file_name;
}

void CDbfTableAdapter::open() {

	open(dbf_file_path.c_str());
}

const Tchar *CDbfTableAdapter::getDbfFilePath() const {

	return dbf_file_path.c_str();
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

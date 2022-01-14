#include "DbfTableAdapter.h"

CDbfTableAdapter::CDbfTableAdapter() : dbf_dir_path_len(0) { }

void CDbfTableAdapter::open(const char *dbf_path) {

	assert(dbf_path && dbf_path[0] != '\0');
	dbf_table.open(dbf_path);
}

void CDbfTableAdapter::open(const wchar_t *dbf_path) {

	assert(dbf_path && dbf_path[0] != L'\0');
	std::vector<char> buffer;
	size_t len = wcslen(dbf_path);	

	buffer.resize(len + 1);
	wcstombs(&buffer[0], dbf_path, len);
	buffer[len] = '\0';

	dbf_table.open(&buffer[0]);
}

const char *CDbfTableAdapter::getCStringCellValue(const size_t field) {

	assert(dbf_table.getFieldType(field) == DBF_FIELD_CHAR);
	return dbf_table.getStrCellValue(field);
}

const wchar_t *CDbfTableAdapter::getCWStringCellValue(const size_t field) {

	assert(dbf_table.getFieldType(field) == DBF_FIELD_CHAR);
	return nullptr;
}

int CDbfTableAdapter::getIntCellValue(size_t field, bool &is_null) {

	const char *value = dbf_table.getNumericCellValue(field);
	if(!value || value[0] == '\0') {
		is_null = true;
		return 0;
	}

	int num_value = atoi(value);
	is_null = false;
	return num_value;
}

const char *CDbfTableAdapter::getNumericCellValue(size_t field) {

	return dbf_table.getNumericCellValue(field);
}

const char *CDbfTableAdapter::getDateCellValue(const size_t field, char *buffer) {

	assert(buffer);
	const char *value = dbf_table.getDateCellValue(field);
	if(!value || value[0] == '\0') return nullptr;

	assert(strlen(value) == 8);
	strncpy(buffer, value, 4);
	strcpy(&buffer[4], "-");
	strncpy(&buffer[5], value, 2);
	strcpy(&buffer[7], "-");
	strncpy(&buffer[8], value, 2);
	return buffer;
}

void CDbfTableAdapter::close() {

	dbf_table.close();
}

CDbfTableAdapter::~CDbfTableAdapter(){

	close();
}

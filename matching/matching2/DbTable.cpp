#include <algorithm>
#include "DbTable.h"
#include <basic/tstring.h>
#include <db/IDbResultSet.h>
#include <db/IDbResultSetMetadata.h>
#include <db/IDbField.h>

CDbRecord::CDbRecord(std::shared_ptr<IDbResultSet> result_set_, \
						const size_t record_no_) : result_set(result_set_), \
													record_no(record_no_){

	assert(result_set);
}

size_t CDbRecord::size() const {

	return result_set->getFieldsCount();
}

const char *CDbRecord::getColValueAsCharArray(const size_t index) const {
	
	result_set->gotoRecord(record_no);
	return result_set->getString(index);
}

CDbRecord::~CDbRecord(){ }

//*****************************************************

CMySQLDbRecord::CMySQLDbRecord(std::shared_ptr<IDbResultSet> result_set_, \
								std::shared_ptr<std::vector<std::shared_ptr<IDbField> > > fields_, \
								const size_t record_no_) : \
							result_set(result_set_), \
							fields(fields_), \
							record_no(record_no_) {

	assert(result_set);
}

size_t CMySQLDbRecord::size() const {

	return result_set->getFieldsCount();
}

const char *CMySQLDbRecord::getStringFromResultSet(wchar_t ,\
								std::shared_ptr<IDbField> field) const{

	auto value = field->getValueAsImmutableWString(result_set);
	if (!value.str) return nullptr;

	buffer.resize(value.size + 1);
#pragma warning(push)
#pragma warning(disable: 4996)
	wcstombs(&buffer[0], value.str, value.size);
	buffer[value.size] = '\0';
#pragma warning(pop)

	return &buffer[0];
}

const char *CMySQLDbRecord::getStringFromResultSet(char ,\
								std::shared_ptr<IDbField> field) const {

	auto value = field->getValueAsImmutableString(result_set);
	return value.str;
}

const char *CMySQLDbRecord::getColValueAsCharArray(const size_t index) const {

	result_set->gotoRecord(record_no);
	return getStringFromResultSet(Tchar(), fields->at(index));
}

CMySQLDbRecord::~CMySQLDbRecord(){ }

//*****************************************************

CDbTable::CDbTable(std::shared_ptr<IDbResultSet> result_set_, \
					std::shared_ptr<IDbResultSetMetadata> metadata) : fields_names(), \
					result_set(result_set_) {

	assert(result_set);
	assert(metadata);
	fields = std::make_shared<std::vector<std::shared_ptr<IDbField> > >();

	size_t fields_count = result_set->getFieldsCount();
	for (size_t i = 0; i < fields_count; ++i) {
		fields->emplace_back(metadata->getField(i));
		fields_names.emplace_back(fields->at(i)->getFieldAlias());
	}
}

size_t CDbTable::getColumnsCount() const {

	return result_set->getFieldsCount();
}

size_t CDbTable::getColIndexByName(const char *col_name) const {

	assert(col_name);
	auto p = std::find(fields_names.cbegin(), fields_names.cend(), col_name);

	return p != fields_names.cend() ? \
			std::distance(fields_names.cbegin(), p) : NOT_FOUND;
}

const char *CDbTable::getColName(const size_t index) const {

	assert(index < fields_names.size());
	return fields_names[index].c_str();
}

size_t CDbTable::getRecordsCount() const {

	return result_set->getRecordsCount();
}

std::shared_ptr<const IRecord> CDbTable::getRecord(const size_t index) const {

	return std::make_shared<const CDbRecord>(result_set, index);
}

std::shared_ptr<IRecord> CDbTable::getRecord(const size_t index) {

	return std::make_shared<CDbRecord>(result_set, index);
}

CDbTable::~CDbTable() { }

//*****************************************************

CMySQLDbTable::CMySQLDbTable(std::shared_ptr<IDbResultSet> result_set_, \
								std::shared_ptr<IDbResultSetMetadata> metadata) : \
							CDbTable(result_set_, metadata) { }

std::shared_ptr<const IRecord> CMySQLDbTable::getRecord(const size_t index) const {

	return std::make_shared<const CMySQLDbRecord>(result_set, fields, index);
}

std::shared_ptr<IRecord> CMySQLDbTable::getRecord(const size_t index) {

	return std::make_shared<CMySQLDbRecord>(result_set, fields, index);
}

CMySQLDbTable::~CMySQLDbTable(){ }
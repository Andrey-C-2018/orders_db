#include "MySQLResultSetMetadata.h"

CMySQLResultSetMetadata::CMySQLResultSetMetadata(std::shared_ptr<MYSQL_RES> metadata_) : \
													metadata(metadata_){

	assert(metadata);
	fields_count = mysql_num_fields(metadata.get());
}

size_t CMySQLResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

CDbField<char> CMySQLResultSetMetadata::getField(const size_t field) const {
	
	return TgetField<char>(field);
}

CDbField<wchar_t> CMySQLResultSetMetadata::getFieldW(const size_t field) const {

	return TgetField<wchar_t>(field);
}

CMySQLResultSetMetadata::~CMySQLResultSetMetadata(){ }

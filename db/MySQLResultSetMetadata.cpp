#include "MySQLResultSetMetadata.h"

CMySQLResultSetMetadata::CMySQLResultSetMetadata(std::shared_ptr<MYSQL_RES> metadata_) : \
													metadata(metadata_){

	assert(metadata);
	fields_count = mysql_num_fields(metadata.get());
}

size_t CMySQLResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

CMySQLResultSetMetadata::~CMySQLResultSetMetadata(){ }

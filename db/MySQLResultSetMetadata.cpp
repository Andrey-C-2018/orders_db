#include "MySQLResultSetMetadata.h"
#include "MySQLField.h"

CMySQLResultSetMetadata::CMySQLResultSetMetadata(std::shared_ptr<MYSQL_RES> metadata_) : \
													metadata(metadata_){

	assert(metadata);
	fields_count = mysql_num_fields(metadata.get());
}

size_t CMySQLResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

std::shared_ptr<IDbField> CMySQLResultSetMetadata::getField(const size_t field) const {

	assert(field < fields_count);

	mysql_field_seek(metadata.get(), (MYSQL_FIELD_OFFSET)field);
	MYSQL_FIELD *mysql_field = mysql_fetch_field(metadata.get());

	if (CMySQLVariant::IsDate(mysql_field->type)) 		
		return std::make_shared<CMySQLDateField>(metadata, field);
	
	if (CMySQLVariant::IsString(mysql_field->type) || \
		CMySQLVariant::IsDecimal(mysql_field->type) || \
		CMySQLVariant::IsEnum(mysql_field->type)) 
		return std::make_shared<CMySQLStringField>(metadata, field);

	switch(mysql_field->type){
		case MYSQL_TYPE_TINY:
			return std::make_shared<CMySQLIntegerField<char> >(metadata, field);

		case MYSQL_TYPE_SHORT:
			return std::make_shared<CMySQLIntegerField<short> >(metadata, field);

		case MYSQL_TYPE_LONG:
			return std::make_shared<CMySQLIntegerField<int> >(metadata, field);
			
		default:
			XException e(0, _T("No such field type: "));
			e << mysql_field->type;
			throw e;
	}

	assert(false);
	return std::shared_ptr<IDbField>();
}

CMySQLResultSetMetadata::~CMySQLResultSetMetadata(){ }

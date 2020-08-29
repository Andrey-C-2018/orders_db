#include "MySQLResultSetMetadata.h"
#include "MySQLField.h"
#include "MySQLVariant.h"
#include "MySQLStmtData.h"

CMySQLMetadataException::CMySQLMetadataException(const int err_code, \
										const Tchar *err_descr) : \
										CMySQLException(err_code, err_descr) { }

CMySQLMetadataException::CMySQLMetadataException(MYSQL *conn) : \
										CMySQLException(conn) { }

CMySQLMetadataException::CMySQLMetadataException(MYSQL_STMT *stmt) : \
										CMySQLException(stmt) { }

CMySQLMetadataException::CMySQLMetadataException(const CMySQLMetadataException &obj) : \
										CMySQLException(obj) { }

CMySQLMetadataException::~CMySQLMetadataException() { }

//*************************************************************

CMySQLResultSetMetadata::CMySQLResultSetMetadata(std::shared_ptr<const MySQLStmtData> stmt_) : \
													stmt(stmt_){

	assert(stmt_);
	assert(stmt_->stmt);
	assert(stmt_->metadata);

	fields_count = mysql_num_fields(stmt_->metadata);
}

size_t CMySQLResultSetMetadata::getFieldsCount() const {

	return fields_count;
}

std::shared_ptr<IDbField> CMySQLResultSetMetadata::getField(const size_t field) const {

	assert(field < fields_count);
	std::shared_ptr<const MySQLStmtData> sh_stmt = stmt.lock();
	if (!sh_stmt)
		throw CMySQLMetadataException(CMySQLMetadataException::E_STMT_RELEASED, \
			_T("the metadata is inaccessible since the stmt has been already released"));

	mysql_field_seek(sh_stmt->metadata, (MYSQL_FIELD_OFFSET)field);
	MYSQL_FIELD *mysql_field = mysql_fetch_field(sh_stmt->metadata);

	if (CMySQLVariant::IsDate(mysql_field->type)) 		
		return std::make_shared<CMySQLDateField>(stmt, field);
	
	if (CMySQLVariant::IsString(mysql_field->type) || \
		CMySQLVariant::IsDecimal(mysql_field->type) || \
		CMySQLVariant::IsEnum(mysql_field->type)) 
		return std::make_shared<CMySQLStringField>(stmt, field);

	switch(mysql_field->type){
		case MYSQL_TYPE_TINY:
			return std::make_shared<CMySQLIntegerField<char> >(stmt, field);

		case MYSQL_TYPE_SHORT:
			return std::make_shared<CMySQLIntegerField<short> >(stmt, field);

		case MYSQL_TYPE_LONG:
			return std::make_shared<CMySQLIntegerField<int> >(stmt, field);
			
		default:
			XException e(0, _T("No such field type: "));
			e << mysql_field->type;
			throw e;
	}

	assert(false);
	return std::shared_ptr<IDbField>();
}

CMySQLResultSetMetadata::~CMySQLResultSetMetadata(){ }

#include <basic/XConv.h>
#include "MySQLField.h"
#include "MySQLResultSet.h"

CMySQLField::CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
							metadata(metadata_), field(field_) { }

CDbFieldProperties<char> CMySQLField::getFieldProperties() const {

	return TgetField<char>();
}

CDbFieldProperties<wchar_t> CMySQLField::getFieldPropertiesW() const {

	return TgetField<wchar_t>();
}

CMySQLField::~CMySQLField() { }

//**************************************************

CMySQLIntegerField::CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
										CMySQLField(metadata_, field_){ }

const char *CMySQLIntegerField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	char buffer[getDigitsCountOfType<int>() + 1];
	int i = result_set->getInt(field);
	int err = 0;

	return XConv::ToString(i, buffer);
}

const wchar_t *CMySQLIntegerField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	wchar_t buffer[getDigitsCountOfType<int>() + 1];
	int i = result_set->getInt(field);
	int err = 0;

	return XConv::ToString(i, buffer);
}

void CMySQLIntegerField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const char *value) const {

}

void CMySQLIntegerField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const wchar_t *value) const {

}

CMySQLIntegerField::~CMySQLIntegerField(){ }
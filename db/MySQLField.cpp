#include "MySQLField.h"

CMySQLField::CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
							metadata(metadata_), field(field_) { }

CDbFieldProperties<std::string> CMySQLField::getFieldProperties() const {
	CDbFieldProperties<std::string> field_props_item;

	TgetField<std::string>(field_props_item);
	return field_props_item;
}

CDbFieldProperties<std::wstring> CMySQLField::getFieldPropertiesW() const {
	CDbFieldProperties<std::wstring> field_props_item;

	TgetField<std::wstring>(field_props_item);
	return field_props_item;
}

CMySQLField::~CMySQLField() { }

//**************************************************

CDbFieldProperties<std::string> CMySQLDateField::getFieldProperties() const {
	CDbFieldProperties<std::string> field_props_item;

	TgetField<std::string>(field_props_item);
	field_props_item.field_size = CDate::GERMAN_FORMAT_LEN;

	return field_props_item;
}

CDbFieldProperties<std::wstring> CMySQLDateField::getFieldPropertiesW() const {
	CDbFieldProperties<std::wstring> field_props_item;

	TgetField<std::wstring>(field_props_item);
	field_props_item.field_size = CDate::GERMAN_FORMAT_LEN;

	return field_props_item;
}

CMySQLDateField::CMySQLDateField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
								CMySQLField(metadata_, field_) { }

const char *CMySQLDateField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return p;
}

const wchar_t *CMySQLDateField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return buffer;
}

ImmutableString<char> CMySQLDateField::getValueAsImmutableString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(p);

	return ImmutableString<char>(p, CDate::GERMAN_FORMAT_LEN);
}

ImmutableString<wchar_t> CMySQLDateField::getValueAsImmutableWString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	CDate date_value = result_set->getDate(field);
	date_value.ToStringGerman(buffer);

	return ImmutableString<wchar_t>(buffer, CDate::GERMAN_FORMAT_LEN);
}

void CMySQLDateField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const char *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no, \
										const wchar_t *value) const {
	CDate date_value(value, CDate::GERMAN_FORMAT);

	assert(!date_value.IsNull());
	binding_target->bindValue(param_no, date_value);
}

void CMySQLDateField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
										std::shared_ptr<IDbBindingTarget> binding_target, \
										const size_t param_no) const {

	CDate date_value = result_set->getDate(field);
	binding_target->bindValue(param_no, date_value);
}

CMySQLDateField::~CMySQLDateField() { }

//**************************************************

CMySQLStringField::CMySQLStringField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
									CMySQLField(metadata_, field_) { }

const char *CMySQLStringField::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getString(field);
}

const wchar_t *CMySQLStringField::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	return result_set->getWString(field);
}

ImmutableString<char> CMySQLStringField::getValueAsImmutableString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableString(field);
}

ImmutableString<wchar_t> CMySQLStringField::getValueAsImmutableWString(\
											const std::shared_ptr<const IDbResultSet> result_set) const {

	return result_set->getImmutableWString(field);
}

void CMySQLStringField::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const char *value) const {
	
	assert(value);
	binding_target->bindValue(param_no, value);
}

void CMySQLStringField::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no, \
											const wchar_t *value) const {
	
	assert(value);
	binding_target->bindValue(param_no, value);
}

void CMySQLStringField::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
											std::shared_ptr<IDbBindingTarget> binding_target, \
											const size_t param_no) const {

	const char *value = result_set->getString(field);
	binding_target->bindValue(param_no, value);
}

CMySQLStringField::~CMySQLStringField() { }
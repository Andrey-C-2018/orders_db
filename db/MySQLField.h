#pragma once
#include <basic/XConv.h>
#include "IDbField.h"
#include "IDbBindingTarget.h"
#include "MySQLResultSet.h"

class CMySQLField : public IDbField{
	std::shared_ptr<MYSQL_RES> metadata;

	mutable std::vector<char> field_name, table_name;
	mutable std::vector<wchar_t> field_name_w, table_name_w;

	bool is_primary_key;
	size_t max_size;

	inline MYSQL_FIELD *getMySQLFieldHandle() const;
protected:
	const size_t field;

	CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, \
				const size_t field_, const size_t max_field_size);
public:
	CMySQLField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	ImmutableString<char> getFieldName() const override;
	ImmutableString<wchar_t> getFieldNameW() const override;
	size_t getFieldMaxLength() const override;
	ImmutableString<char> getTableName() const override;
	ImmutableString<wchar_t> getTableNameW() const override;
	bool isPrimaryKey() const override;
	
	virtual ~CMySQLField();
};

template <typename Tint> \
class CMySQLIntegerField : public CMySQLField {
	mutable wchar_t buffer[getDigitsCountOfType<Tint>() + 1];
public:
	CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLIntegerField();
};

class CMySQLDateField : public CMySQLField {
	mutable wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CMySQLDateField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLDateField();
};

class CMySQLStringField : public CMySQLField {
	wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CMySQLStringField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_);

	const char *getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	const wchar_t *getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<char> getValueAsImmutableString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;
	ImmutableString<wchar_t> getValueAsImmutableWString(\
									const std::shared_ptr<const IDbResultSet> result_set) const override;

	void bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const char *value) const override;
	void bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no, \
							const wchar_t *value) const override;

	void getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
							std::shared_ptr<IDbBindingTarget> binding_target, \
							const size_t param_no) const override;

	virtual ~CMySQLStringField();
};

//**************************************************

MYSQL_FIELD *CMySQLField::getMySQLFieldHandle() const {

	mysql_field_seek(metadata.get(), (MYSQL_FIELD_OFFSET)field);
	return mysql_fetch_field(metadata.get());
}

//**************************************************

template <typename Tint> \
CMySQLIntegerField<Tint>::CMySQLIntegerField(std::shared_ptr<MYSQL_RES> metadata_, const size_t field_) : \
											CMySQLField(metadata_, field_, getDigitsCountOfType<Tint>()) { }

template <typename Tint> \
const char *CMySQLIntegerField<Tint>::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;

	return XConv::ToString(result_set->getInt(field), p);
}

template <typename Tint> \
const wchar_t *CMySQLIntegerField<Tint>::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {

	return XConv::ToString(result_set->getInt(field), buffer);
}

template <typename Tint> \
ImmutableString<char> CMySQLIntegerField<Tint>::getValueAsImmutableString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;
	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), p, size);
	return ImmutableString<char>(str, size);
}

template <typename Tint> \
ImmutableString<wchar_t> CMySQLIntegerField<Tint>::getValueAsImmutableWString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	size_t size = 0;

	auto str = XConv::ToString(result_set->getInt(field), buffer, size);
	return ImmutableString<wchar_t>(str, size);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const char *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	assert(!err);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const wchar_t *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	assert(!err);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
												std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no) const {
	Tint i = result_set->getInt(field);
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
CMySQLIntegerField<Tint>::~CMySQLIntegerField() { }

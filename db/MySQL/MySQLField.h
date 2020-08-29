#pragma once
#include <basic/XConv.h>
#include "../IDbField.h"
#include "../IDbBindingTarget.h"
#include "../IDbResultSet.h"
#include "MySQLException.h"

class CMySQLFieldException : public CMySQLException {
public:
	enum Errors {
		E_STMT_RELEASED = 1
	};

	CMySQLFieldException(const int err_code, const Tchar *err_descr);
	CMySQLFieldException(MYSQL *conn);
	CMySQLFieldException(MYSQL_STMT *stmt);

	CMySQLFieldException(const CMySQLFieldException &obj);
	CMySQLFieldException(CMySQLFieldException &&obj) = default;

	~CMySQLFieldException();
};

struct MySQLStmtData;

class CMySQLField : public IDbField{
	std::weak_ptr<const MySQLStmtData> stmt;

	bool is_primary_key;
	size_t max_size;

	MYSQL_FIELD *getMySQLFieldHandle() const;

protected:
	const size_t field;

	CMySQLField(std::weak_ptr<const MySQLStmtData> stmt_, \
				const size_t field_, const size_t max_field_size);
public:
	CMySQLField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_);

	CMySQLField(const CMySQLField &obj) = delete;
	CMySQLField(CMySQLField &&obj) = default;
	CMySQLField &operator=(const CMySQLField &obj) = delete;
	CMySQLField &operator=(CMySQLField &&obj) = default;

	std::string getFieldName() const override;
	std::wstring getFieldNameW() const override;
	std::string getFieldAlias() const override;
	std::wstring getFieldAliasW() const override;
	size_t getFieldMaxLength() const override;
	std::string getTableName() const override;
	std::wstring getTableNameW() const override;
	bool isPrimaryKey() const override;
	
	virtual ~CMySQLField();
};

template <typename Tint> \
class CMySQLIntegerField : public CMySQLField {
	mutable wchar_t buffer[getDigitsCountOfType<Tint>() + 1];
public:
	CMySQLIntegerField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_);

	CMySQLIntegerField(const CMySQLField &obj) = delete;
	CMySQLIntegerField(CMySQLIntegerField &&obj) = default;
	CMySQLIntegerField &operator=(const CMySQLIntegerField &obj) = delete;
	CMySQLIntegerField &operator=(CMySQLIntegerField &&obj) = default;

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

	const char *getQuoteStr() const override;

	virtual ~CMySQLIntegerField();
};

class CMySQLDateField : public CMySQLField {
	mutable wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	CMySQLDateField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_);

	CMySQLDateField(const CMySQLDateField &obj) = delete;
	CMySQLDateField(CMySQLDateField &&obj) = default;
	CMySQLDateField &operator=(const CMySQLDateField &obj) = delete;
	CMySQLDateField &operator=(CMySQLDateField &&obj) = default;

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

	const char *getQuoteStr() const override;

	virtual ~CMySQLDateField();
};

class CMySQLStringField : public CMySQLField {
public:
	CMySQLStringField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_);

	CMySQLStringField(const CMySQLStringField &obj) = delete;
	CMySQLStringField(CMySQLStringField &&obj) = default;
	CMySQLStringField &operator=(const CMySQLStringField &obj) = delete;
	CMySQLStringField &operator=(CMySQLStringField &&obj) = default;

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

	const char *getQuoteStr() const override;

	virtual ~CMySQLStringField();
};

//**************************************************

template <typename Tint> \
CMySQLIntegerField<Tint>::CMySQLIntegerField(std::weak_ptr<const MySQLStmtData> stmt_, const size_t field_) : \
											CMySQLField(stmt_, field_, getDigitsCountOfType<Tint>()) { }

template <typename Tint> \
const char *CMySQLIntegerField<Tint>::getValueAsString(const std::shared_ptr<const IDbResultSet> result_set) const {
	
	char *p = (char *)buffer;
	bool is_null = false;
	Tint int_value = result_set->getInt(field, is_null);

	return is_null ? nullptr : XConv::ToString(int_value, p);
}

template <typename Tint> \
const wchar_t *CMySQLIntegerField<Tint>::getValueAsWString(const std::shared_ptr<const IDbResultSet> result_set) const {

	bool is_null = false;
	Tint int_value = result_set->getInt(field, is_null);

	return is_null ? nullptr : XConv::ToString(int_value, buffer);
}

template <typename Tint> \
ImmutableString<char> CMySQLIntegerField<Tint>::getValueAsImmutableString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	char *p = (char *)buffer;
	size_t size = 0;

	bool is_null = false;
	Tint int_value = result_set->getInt(field, is_null);

	const char *str = is_null ? nullptr : XConv::ToString(int_value, p, size);
	return ImmutableString<char>(str, size);
}

template <typename Tint> \
ImmutableString<wchar_t> CMySQLIntegerField<Tint>::getValueAsImmutableWString(\
										const std::shared_ptr<const IDbResultSet> result_set) const {
	size_t size = 0;

	bool is_null = false;
	Tint int_value = result_set->getInt(field, is_null);

	const wchar_t *str = is_null ? nullptr : XConv::ToString(int_value, buffer, size);
	return ImmutableString<wchar_t>(str, size);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const char *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	if (err) {
		CMySQLFieldException e(CDbException::E_DB_WRONG_VALUE, \
							_T("the binding value is not an integer: "));
		e << value << _T(" , param no: ")<< param_no;
		throw e;
	}
	
	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::bindValueAsWString(std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no, \
												const wchar_t *value) const {
	int err = 0;
	Tint i = XConv::ToInt(value, err);

	if (err) {
		CMySQLFieldException e(CDbException::E_DB_WRONG_VALUE, \
							_T("the binding value is not an integer: "));
		e << value << _T(" , param no: ") << param_no;
		throw e;
	}

	binding_target->bindValue(param_no, i);
}

template <typename Tint> \
void CMySQLIntegerField<Tint>::getValueAndBindItTo(const std::shared_ptr<const IDbResultSet> result_set, \
												std::shared_ptr<IDbBindingTarget> binding_target, \
												const size_t param_no) const {
	bool is_null = false;
	Tint i = result_set->getInt(field, is_null);

	if (is_null)
		binding_target->bindNull(param_no);
	else
		binding_target->bindValue(param_no, i);
}

template <typename Tint> \
const char *CMySQLIntegerField<Tint>::getQuoteStr() const {

	return "";
}

template <typename Tint> \
CMySQLIntegerField<Tint>::~CMySQLIntegerField() { }

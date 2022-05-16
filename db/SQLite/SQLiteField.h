#pragma once
#include <basic/XConv.h>
#include "../IDbField.h"
#include "../IDbBindingTarget.h"
#include "SQLiteResultSet.h"

class DBLIB_DLL_EXPORT SQLiteFieldException : public SQLiteException {
public:
	SQLiteFieldException(const int err_code, const Tchar *err_descr);
	SQLiteFieldException(sqlite3 *db);

	SQLiteFieldException(const SQLiteFieldException &obj);
	SQLiteFieldException(SQLiteFieldException &&obj) = default;

	virtual ~SQLiteFieldException();
};

class SQLiteField : public IDbField{
	std::shared_ptr<sqlite3> db;
	std::shared_ptr<Statement> stmt;
	size_t max_length;

	static inline size_t getFieldMaxLengthByType(const char *field_type);
protected:
	const size_t field;

public:
	SQLiteField(std::shared_ptr<sqlite3> db_, \
				std::shared_ptr<Statement> stmt_, \
				const size_t field_, const char *field_type);
	SQLiteField(std::shared_ptr<sqlite3> db_, \
				std::shared_ptr<Statement> stmt_, \
				const size_t field_, const char *field_type, const size_t def_max_length);
	SQLiteField(std::shared_ptr<sqlite3> db_, \
				std::shared_ptr<Statement> stmt_, \
				const size_t field_, const size_t def_max_length);
	
	SQLiteField(const SQLiteField &obj) = delete;
	SQLiteField(SQLiteField &&obj) = default;
	SQLiteField &operator=(const SQLiteField &obj) = delete;
	SQLiteField &operator=(SQLiteField &&obj) = default;

	std::string getFieldName() const override;
	std::wstring getFieldNameW() const override;
	std::string getFieldAlias() const override;
	std::wstring getFieldAliasW() const override;
	size_t getFieldMaxLength() const override;
	std::string getTableName() const override;
	std::wstring getTableNameW() const override;
	bool isPrimaryKey() const override;
	
	virtual ~SQLiteField();
};

class SQLiteIntegerField : public SQLiteField {
	mutable wchar_t buffer[getDigitsCountOfType<int>() + 1];
public:
	SQLiteIntegerField(std::shared_ptr<sqlite3> db_, \
						std::shared_ptr<Statement> stmt_, \
						const size_t field_, const char *field_type);

	SQLiteIntegerField(const SQLiteIntegerField &obj) = delete;
	SQLiteIntegerField(SQLiteIntegerField &&obj) = default;
	SQLiteIntegerField &operator=(const SQLiteIntegerField &obj) = delete;
	SQLiteIntegerField &operator=(SQLiteIntegerField &&obj) = default;

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

	virtual ~SQLiteIntegerField();
};

class SQLiteDateField : public SQLiteField {
	mutable wchar_t buffer[CDate::GERMAN_FORMAT_LEN + 1];
public:
	SQLiteDateField(std::shared_ptr<sqlite3> db_, \
					std::shared_ptr<Statement> stmt_, const size_t field_);

	SQLiteDateField(const SQLiteDateField &obj) = delete;
	SQLiteDateField(SQLiteDateField &&obj) = default;
	SQLiteDateField &operator=(const SQLiteDateField &obj) = delete;
	SQLiteDateField &operator=(SQLiteDateField &&obj) = default;

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

	virtual ~SQLiteDateField();
};

class SQLiteStringField : public SQLiteField {
public:
	SQLiteStringField(std::shared_ptr<sqlite3> db_, \
						std::shared_ptr<Statement> stmt_, \
						const size_t field_, const char *field_type);

	SQLiteStringField(const SQLiteStringField &obj) = delete;
	SQLiteStringField(SQLiteStringField &&obj) = default;
	SQLiteStringField &operator=(const SQLiteStringField &obj) = delete;
	SQLiteStringField &operator=(SQLiteStringField &&obj) = default;

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

	virtual ~SQLiteStringField();
};

//**************************************************

size_t SQLiteField::getFieldMaxLengthByType(const char *field_type) {

	auto p = strchr(field_type, '(');
	if (p) {
		int error = 0;
		size_t max_length = XConv::ToSizeType(p + 1, error);
		assert(error == XConv::E_CONV_WRONG_VALUE);
		return max_length;
	}

	return (size_t)-1;
}
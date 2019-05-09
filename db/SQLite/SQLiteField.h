#pragma once
#include <basic/XConv.h>
#include "../IDbField.h"
#include "../IDbBindingTarget.h"
#include "SQLiteResultSet.h"

class SQLiteField : public IDbField{
public:
	SQLiteField(std::shared_ptr<SQLiteStmtHandle> metadata_, const size_t field_);

	SQLiteField(const SQLiteField &obj) = delete;
	SQLiteField(SQLiteField &&obj) = default;
	SQLiteField &operator=(const SQLiteField &obj) = delete;
	SQLiteField &operator=(SQLiteField &&obj) = default;

	ImmutableString<char> getFieldName() const override;
	ImmutableString<wchar_t> getFieldNameW() const override;
	size_t getFieldMaxLength() const override;
	ImmutableString<char> getTableName() const override;
	ImmutableString<wchar_t> getTableNameW() const override;
	bool isPrimaryKey() const override;
	
	virtual ~SQLiteField();
};

class SQLiteIntegerField : public SQLiteField {
public:
	SQLiteIntegerField(std::shared_ptr<SQLiteStmtHandle> metadata_, const size_t field_);

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

	virtual ~SQLiteIntegerField();
};

class SQLiteDateField : public SQLiteField {
public:
	SQLiteDateField(std::shared_ptr<SQLiteStmtHandle> metadata_, const size_t field_);

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

	virtual ~SQLiteDateField();
};

class SQLiteStringField : public SQLiteField {
public:
	SQLiteStringField(std::shared_ptr<SQLiteStmtHandle> metadata_, const size_t field_);

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

	virtual ~SQLiteStringField();
};

//**************************************************


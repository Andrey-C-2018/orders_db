#pragma once
#include <memory>
#include <vector>
#include "IDbResultSet.h"
#include "MySQLCommon.h"

class CMySQLResultSetException : public CMySQLException {
public:
	CMySQLResultSetException(const int err_code, const Tchar *err_descr);
	CMySQLResultSetException(MYSQL *conn);
	CMySQLResultSetException(MYSQL_STMT *stmt);

	CMySQLResultSetException(const CMySQLResultSetException &obj);
	CMySQLResultSetException(CMySQLResultSetException &&obj) = default;

	~CMySQLResultSetException();
};

class CMySQLResultSet : public IDbResultSet {
	size_t fields_count, records_count;
	mutable std::shared_ptr<MYSQL_STMT> stmt;
	std::shared_ptr<MYSQL_RES> metadata;
	mutable size_t curr_record;

	std::vector<CMySQLBindingTarget> fields;
	std::vector<MYSQL_BIND> fields_bindings;

	void goto_record(const size_t record) const;
	inline void goto_rec_and_update(const size_t field, const size_t record) const;
public:
	CMySQLResultSet(std::shared_ptr<MYSQL_STMT> stmt_, 
						std::shared_ptr<MYSQL_RES> metadata_);

	CMySQLResultSet(const CMySQLResultSet &obj) = delete;
	CMySQLResultSet(CMySQLResultSet &&obj);
	CMySQLResultSet &operator=(const CMySQLResultSet &obj) = delete;
	CMySQLResultSet &operator=(CMySQLResultSet &&obj);

	bool empty() const override;
	size_t getFieldsCount() const override;
	size_t getRecordsCount() const override;

	int getInt(const size_t field, const size_t record) const override;
	const char *getString(const size_t field, \
							const size_t record) const override;
	const wchar_t *getWString(const size_t field, \
								const size_t record) const override;
	ImmutableString<char> getImmutableString(const size_t field, \
												const size_t record) const override;
	ImmutableString<wchar_t> getImmutableWString(const size_t field, \
													const size_t record) const override;
	CDate getDate(const size_t field, const size_t record) const override;

	void getValueAndBindItTo(const size_t field, const size_t record, \
							const size_t binding_param_no, \
							std::shared_ptr<IDbBindingTarget> binding_target) const override;

	void Release();
	virtual ~CMySQLResultSet();
};

//*************************************************************

void CMySQLResultSet::goto_rec_and_update(const size_t field, \
											const size_t record) const {

	if (curr_record != record) goto_record(record);
	fields[field].value.UpdateLength(fields[field].length);
}

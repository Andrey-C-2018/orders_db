#pragma once
#include <vector>
#include <db/IDbResultSet.h>
#include "MySQLVariant.h"

class MySQLStaticResultSet : public IDbResultSet {

	size_t fields_count;
	std::vector<std::vector<CMySQLVariant> > data;
	mutable size_t curr_record;

public:
	MySQLStaticResultSet(size_t fields_count_, size_t records_count);

	MySQLStaticResultSet(const MySQLStaticResultSet &obj) = delete;
	MySQLStaticResultSet(MySQLStaticResultSet &&obj) = default;
	MySQLStaticResultSet& operator=(const MySQLStaticResultSet &obj) = delete;
	MySQLStaticResultSet& operator=(MySQLStaticResultSet &&obj) = default;

	bool empty() const override;
	size_t getFieldsCount() const override;
	size_t getRecordsCount() const override;

	void setValue(size_t field, size_t record, CMySQLVariant value);

	void gotoRecord(const size_t record) const override;

	int getInt(const size_t field, bool &is_null) const override;
	const char *getString(const size_t field) const override;
	const wchar_t *getWString(const size_t field) const override;
	ImmutableString<char> getImmutableString(const size_t field) const override;
	ImmutableString<wchar_t> getImmutableWString(const size_t field) const override;
	CDate getDate(const size_t field, bool &is_null) const override;

	void reload() override;
	std::shared_ptr<IDbResultSet> staticClone() const override;

	virtual ~MySQLStaticResultSet();
};

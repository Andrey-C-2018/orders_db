#pragma once
#include <memory>
#include <db/IDbResultSet.h>
#include "StmtData.h"

class Statement;

class ResultSet : public IDbResultSet {
	StmtData data;
	std::shared_ptr<IDbResultSet> mysql_rs;

public:
	ResultSet(StmtData data_, \
				std::shared_ptr<IDbResultSet> mysql_rs_);

	ResultSet(const ResultSet &obj) = delete;
	ResultSet(ResultSet &&obj) noexcept;
	ResultSet &operator=(const ResultSet &obj) = delete;
	ResultSet &operator=(ResultSet &&obj) noexcept;

	bool empty() const override;
	size_t getFieldsCount() const override;
	size_t getRecordsCount() const override;

	void gotoRecord(const size_t record) const override;

	int getInt(const size_t field, bool &is_null) const override;
	const char *getString(const size_t field) const override;
	const wchar_t *getWString(const size_t field) const override;
	ImmutableString<char> getImmutableString(const size_t field) const override;
	ImmutableString<wchar_t> getImmutableWString(const size_t field) const override;
	CDate getDate(const size_t field, bool &is_null) const override;

	void reload() override;

	virtual ~ResultSet() { }
};

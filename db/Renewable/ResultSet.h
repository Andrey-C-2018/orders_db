#pragma once
#include "../IDbResultSet.h"

class ResultSet : public IDbResultSet {
public:
	bool empty() const;
	size_t getFieldsCount() const;
	size_t getRecordsCount() const;

	void gotoRecord(const size_t record) const;

	int getInt(const size_t field, bool &is_null) const;
	const char *getString(const size_t field) const;
	const wchar_t *getWString(const size_t field) const;
	ImmutableString<char> getImmutableString(const size_t field) const;
	ImmutableString<wchar_t> getImmutableWString(const size_t field) const;
	CDate getDate(const size_t field, bool &is_null) const;

	void reload();

	~ResultSet() { }
};

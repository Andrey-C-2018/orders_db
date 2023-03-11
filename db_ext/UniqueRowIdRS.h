#pragma once
#include <unordered_set>
#include <db/IDbResultSet.h>
#include "PaginalRS.h"

class UniqueRowIdRS : public IDbResultSet {
	const int field_id_index;
	mutable bool not_unique;
	PaginalRS rs;
	mutable std::unordered_set<int> rec_ids;

public:
	UniqueRowIdRS(PaginalRS rs, int field_id_index_);

	UniqueRowIdRS(const UniqueRowIdRS &obj) = delete;
	UniqueRowIdRS(UniqueRowIdRS &&obj) = default;
	UniqueRowIdRS& operator=(const UniqueRowIdRS &obj) = delete;
	UniqueRowIdRS& operator=(UniqueRowIdRS &&obj) = delete;

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

	virtual ~UniqueRowIdRS();
};

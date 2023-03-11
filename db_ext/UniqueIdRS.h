#pragma once
#include <db/IDbResultSet.h>

class UniqueIdRS : public IDbResultSet {
	const int field_id_index;
	bool not_unique;
	CumulativeRS rs;

public:
	UniqueIdRS(CumulativeRS rs, int field_id_index_);

	UniqueIdRS(const UniqueIdRS &obj) = delete;
	UniqueIdRS(UniqueIdRS &&obj) = default;
	UniqueIdRS& operator=(const UniqueIdRS &obj) = delete;
	UniqueIdRS& operator=(UniqueIdRS &&obj) = default;

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

	virtual ~UniqueIdRS();
};

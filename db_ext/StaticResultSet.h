#pragma once
#include <vector>
#include <unordered_set>
#include <basic/ImmutableString.h>
#include <date/Variant.h>
#include <db/IDbStaticResultSet.h>

class StaticResultSet final : public IDbStaticResultSet {
	size_t fields_count;
	size_t field_rec_id;
	std::vector<std::vector<Variant> > data;
	std::unordered_set<int> *rec_ids;
	bool is_double;
	mutable size_t curr_record;

public:
	StaticResultSet(size_t field_rec_id, std::unordered_set<int> &rec_ids_);

	void init(size_t fields_count_, size_t records_count) override;

	StaticResultSet(const StaticResultSet &obj) = delete;
	StaticResultSet(StaticResultSet &&obj) = default;
	StaticResultSet& operator=(const StaticResultSet &obj) = delete;
	StaticResultSet& operator=(StaticResultSet &&obj) = default;

	inline bool empty() const;
	inline size_t getFieldsCount() const;
	inline size_t getRecordsCount() const;

	size_t getFieldRecId() const override;
	bool gotoRecord(size_t record, int field_id_value) override;
	void setValue(size_t field, Variant value) override;

	inline void gotoRecord(const size_t record) const;

	inline int getInt(const size_t field, bool &is_null) const;
	inline const char *getString(const size_t field) const;
	inline const wchar_t *getWString(const size_t field) const;
	inline ImmutableString<char> getImmutableString(const size_t field) const;
	inline ImmutableString<wchar_t> getImmutableWString(const size_t field) const;
	inline CDate getDate(const size_t field, bool &is_null) const;

	~StaticResultSet();
};

//**************************************************************

bool StaticResultSet::empty() const {

	return data.empty();
}

size_t StaticResultSet::getFieldsCount() const {

	return fields_count;
}

size_t StaticResultSet::getRecordsCount() const {

	return data.size();
}

void StaticResultSet::gotoRecord(const size_t record) const {

	curr_record = record;
}

int StaticResultSet::getInt(const size_t field, bool &is_null) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	is_null = data_item.isNull();
	return is_null ? -1 : data_item.getInt();
}

const char *StaticResultSet::getString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	return data_item.isNull() ? nullptr : data_item.getString();
}

const wchar_t *StaticResultSet::getWString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	return data_item.isNull() ? nullptr : data_item.getWString();
}

ImmutableString<char> StaticResultSet::getImmutableString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto& data_item = data[curr_record][field];
	if (data_item.isNull()) return ImmutableString<char>(nullptr, 0);

	auto str = data_item.getString();
	size_t len = data_item.getStringLength();

	return ImmutableString<char>(str, len);
}

ImmutableString<wchar_t> StaticResultSet::getImmutableWString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	if (data_item.isNull()) return ImmutableString<wchar_t>(nullptr, 0);

	auto str = data_item.getWString();
	size_t len = data_item.getStringLength();

	return ImmutableString<wchar_t>(str, len);
}

CDate StaticResultSet::getDate(const size_t field, bool &is_null) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	is_null = data_item.isNull();
	return is_null ? CDate() : data_item.getDate();
}

#include <memory>
#include "MySQLStaticResultSet.h"

MySQLStaticResultSet::MySQLStaticResultSet(size_t fields_count_, size_t records_count) : \
							fields_count(fields_count_), curr_record((size_t)-1) {

	std::vector<CMySQLVariant> record_data;
	record_data.resize(fields_count);

	data.reserve(records_count);
	data.insert(data.end(), records_count, record_data);
}

bool MySQLStaticResultSet::empty() const {

	return data.empty();
}

size_t MySQLStaticResultSet::getFieldsCount() const {

	return fields_count;
}

size_t MySQLStaticResultSet::getRecordsCount() const {

	return data.size();
}

void MySQLStaticResultSet::setValue(size_t field, size_t record, CMySQLVariant value) {

	assert(field < fields_count);
	assert(record < data.size());
	auto &data_item = data[record][field];

	data_item = std::move(value);
}

void MySQLStaticResultSet::gotoRecord(const size_t record) const {

	curr_record = record;
}

int MySQLStaticResultSet::getInt(const size_t field, bool &is_null) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	is_null = data_item.IsNull();
	return is_null ? -1 : data_item.GetInt();
}

const char *MySQLStaticResultSet::getString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	return data_item.IsNull() ? nullptr : data_item.GetString();
}

const wchar_t *MySQLStaticResultSet::getWString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	bool is_null = data_item.IsNull();
	return is_null ? nullptr : data_item.GetWString();
}

ImmutableString<char> MySQLStaticResultSet::getImmutableString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto& data_item = data[curr_record][field];
	if (data_item.IsNull()) return ImmutableString<char>(nullptr, 0);

	auto str = data_item.GetString();
	size_t len = data_item.GetValueLength();

	return ImmutableString<char>(str, len);
}

ImmutableString<wchar_t> MySQLStaticResultSet::getImmutableWString(const size_t field) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	if (data_item.IsNull()) return ImmutableString<wchar_t>(nullptr, 0);

	auto str = data_item.GetWString();
	size_t len = data_item.GetValueLength();

	return ImmutableString<wchar_t>(str, len);
}

CDate MySQLStaticResultSet::getDate(const size_t field, bool &is_null) const {

	assert(curr_record != (size_t)-1);
	assert(field < fields_count);

	auto &data_item = data[curr_record][field];
	is_null = data_item.IsNull();
	return is_null ? CDate() : data_item.GetDate();
}

void MySQLStaticResultSet::reload() { }

std::shared_ptr<IDbResultSet> MySQLStaticResultSet::staticClone() const {

	return nullptr;
}

MySQLStaticResultSet::~MySQLStaticResultSet() { }

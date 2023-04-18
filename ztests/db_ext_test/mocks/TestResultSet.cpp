#include <cassert>
#include <algorithm>
#include <db/IDbStaticResultSet.h>
#include "TestResultSet.h"
#include "ITestStatement.h"

TestResultSet::TestResultSet(ITestStatement *stmt_) noexcept : \
							stmt(stmt_), \
							fields_count(0), curr_rec((size_t)-1) {

	assert (stmt);
}

void TestResultSet::init(const int *values_, size_t fields, size_t records) {

	assert (fields || (!fields && !records && !values_));
	fields_count = fields;
	this->values.clear();

	if (records > 0) {
		this->values.reserve(records);

		for (size_t i = 0; i < records; i++) {
			std::vector<int> rec(values_ + fields * i, values_ + fields * (i + 1));
			this->values.emplace_back(std::move(rec));
		}
	}
	curr_rec = (size_t)-1;
}

bool TestResultSet::empty() const {

	return values.empty();
}

size_t TestResultSet::getFieldsCount() const {

	return fields_count;
}

size_t TestResultSet::getRecordsCount() const {

	return values.size();
}

void TestResultSet::gotoRecord(const size_t record) const {

	assert (record < values.size());
	curr_rec = record;
}

int TestResultSet::getInt(const size_t field, bool &is_null) const {

	assert (curr_rec != (size_t)-1);
	assert (field < fields_count);

	is_null = false;
	return values[curr_rec][field];
}

const char *TestResultSet::getString(const size_t field) const {

	return nullptr;
}

const wchar_t *TestResultSet::getWString(const size_t field) const {

	return nullptr;
}

ImmutableString<char> TestResultSet::getImmutableString(const size_t field) const {

	return ImmutableString<char>(nullptr, 0);
}

ImmutableString<wchar_t> TestResultSet::getImmutableWString(const size_t field) const {

	return ImmutableString<wchar_t>(nullptr, 0);
}

CDate TestResultSet::getDate(const size_t field, bool &is_null) const {

	is_null = true;
	return CDate();
}

void TestResultSet::reload() {

	stmt->reloadRS();
}

void TestResultSet::upload(IDbStaticResultSet &static_rs) const {

	static_rs.init(fields_count, values.size());
	size_t field_id_rec = static_rs.getFieldRecId();

	for (curr_rec = 0; curr_rec < values.size(); curr_rec++) {

		int id = values[curr_rec][field_id_rec];
		static_rs.gotoRecord(curr_rec, id);

		for (size_t j = 0; j < fields_count; j++) {

			Variant v(values[curr_rec][j]);
			static_rs.setValue(j, std::move(v));
		}
	}
}

TestResultSet::~TestResultSet() { }

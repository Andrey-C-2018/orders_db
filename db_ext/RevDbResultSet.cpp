#include "RevDbResultSet.h"

CRevDbResultSet::CRevDbResultSet(std::shared_ptr<IDbResultSet> actual_rs_, \
									const bool rev_order_) : \
							actual_rs(actual_rs_), rev_order(rev_order_) {

	assert(actual_rs);
	records_count = actual_rs->getRecordsCount();
}

bool CRevDbResultSet::empty() const {

	return actual_rs->empty();
}

size_t CRevDbResultSet::getFieldsCount() const {

	return actual_rs->getFieldsCount();
}

size_t CRevDbResultSet::getRecordsCount() const {

	return records_count;
}

void CRevDbResultSet::gotoRecord(const size_t record) const {

	size_t has_records = records_count > 0;
	size_t rev_record = has_records * (records_count - record - 1);

	size_t actual_record = rev_order ? rev_record : record;
	actual_rs->gotoRecord(actual_record);
}

int CRevDbResultSet::getInt(const size_t field, bool &is_null) const {

	return actual_rs->getInt(field, is_null);
}

const char *CRevDbResultSet::getString(const size_t field) const {

	return actual_rs->getString(field);
}

const wchar_t *CRevDbResultSet::getWString(const size_t field) const {

	return actual_rs->getWString(field);
}

ImmutableString<char> CRevDbResultSet::getImmutableString(const size_t field) const {

	return actual_rs->getImmutableString(field);
}

ImmutableString<wchar_t> CRevDbResultSet::getImmutableWString(const size_t field) const {

	return actual_rs->getImmutableWString(field);
}

CDate CRevDbResultSet::getDate(const size_t field, bool &is_null) const {

	return actual_rs->getDate(field, is_null);
}

void CRevDbResultSet::reload() {

	actual_rs->reload();
	records_count = actual_rs->getRecordsCount();
}

CRevDbResultSet::~CRevDbResultSet() { }

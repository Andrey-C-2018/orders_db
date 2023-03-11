#include "UniqueRowIdRS.h"

UniqueRowIdRS::UniqueRowIdRS(PaginalRS rs_, int field_id_index_) : \
		field_id_index(field_id_index_), not_unique(false), \
		rs(std::move(rs_)) { }

bool UniqueRowIdRS::empty() const {

	return rs.empty();
}

size_t UniqueRowIdRS::getFieldsCount() const {

	return rs.getFieldsCount();
}

size_t UniqueRowIdRS::getRecordsCount() const {

	return rs.getRecordsCount();
}

void UniqueRowIdRS::gotoRecord(const size_t record) const {

	rs.gotoRecord(record);
	bool is_null;
	int rec_id = rs.getInt(field_id_index, is_null);

	auto p = rec_ids.insert(rec_id);
	not_unique = p.second;
}

int UniqueRowIdRS::getInt(const size_t field, bool &is_null) const {

	is_null = not_unique;
	return not_unique ? 0 : rs.getInt(field, is_null);
}

const char *UniqueRowIdRS::getString(const size_t field) const {

	return not_unique ? nullptr : rs.getString(field);
}

const wchar_t *UniqueRowIdRS::getWString(const size_t field) const {

	return not_unique ? nullptr : rs.getWString(field);
}

ImmutableString<char> UniqueRowIdRS::getImmutableString(const size_t field) const {

	return not_unique ? ImmutableString<char>(nullptr, 0) : \
	        			rs.getImmutableString(field);
}

ImmutableString<wchar_t> UniqueRowIdRS::getImmutableWString(const size_t field) const {

	return not_unique ? ImmutableString<wchar_t>(nullptr, 0) : \
	        			rs.getImmutableWString(field);
}

CDate UniqueRowIdRS::getDate(const size_t field, bool &is_null) const {

	is_null = not_unique;
	return not_unique ? CDate() : rs.getDate(field, is_null);
}

void UniqueRowIdRS::reload() {

	rec_ids.clear();
	rs.reload();
}

UniqueRowIdRS::~UniqueRowIdRS() { }

#pragma once
#include <unordered_map>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>

class PaginalRS final {
	static constexpr size_t PAGE_SIZE = 100;
	size_t param_limit_size; // LIMIT row_count OFFSET offset_in_rs

	mutable std::shared_ptr<IDbStatement> stmt;
	std::shared_ptr<IDbStatement> stmt_rec_count;
	size_t rec_count;
	mutable size_t curr_page_first_rec;
	mutable std::unordered_map<size_t, std::shared_ptr<IDbResultSet> > rs_cache;
	mutable std::shared_ptr<IDbResultSet> rs, curr_rs;
	std::shared_ptr<IDbResultSet> rs_rec_count;

	inline void addRSToCache() const;

public:
	PaginalRS(std::shared_ptr<IDbStatement> stmt_,
			  std::shared_ptr<IDbStatement> stmt_rec_count_);

	PaginalRS(const PaginalRS &obj) = delete;
	PaginalRS(PaginalRS &&obj) = default;
	PaginalRS& operator=(const PaginalRS &obj) = delete;
	PaginalRS& operator=(PaginalRS &&obj) = default;

	inline bool empty() const;
	inline size_t getFieldsCount() const;
	inline size_t getRecordsCount() const;

	inline void gotoRecord(const size_t record) const;

	inline int getInt(const size_t field, bool &is_null) const;
	inline const char *getString(const size_t field) const;
	inline const wchar_t *getWString(const size_t field) const;
	inline ImmutableString<char> getImmutableString(const size_t field) const;
	inline ImmutableString<wchar_t> getImmutableWString(const size_t field) const;
	inline CDate getDate(const size_t field, bool &is_null) const;

	inline void reload();
};

//*****************************************************

bool PaginalRS::empty() const {

	return rec_count == 0;
}

size_t PaginalRS::getFieldsCount() const {

	return rs->getFieldsCount();
}

size_t PaginalRS::getRecordsCount() const {

	return rec_count;
}

void PaginalRS::gotoRecord(const size_t record) const {

	assert(record < rec_count);
	if (curr_page_first_rec > record || record > curr_page_first_rec + PAGE_SIZE) {

		curr_page_first_rec = (record / PAGE_SIZE) * PAGE_SIZE;
		auto p = rs_cache.find(curr_page_first_rec);
		if (p != rs_cache.end())
			curr_rs = p->second;
		else {

			stmt->bindValue(param_limit_size, (int) PAGE_SIZE);
			stmt->bindValue(param_limit_size + 1, (int) curr_page_first_rec);
			rs->reload();

			addRSToCache();
		}
	}

	curr_rs->gotoRecord(record % PAGE_SIZE);
}

void PaginalRS::addRSToCache() const {

	auto cloned = rs->staticClone();
	assert (cloned != nullptr);

	auto pair = rs_cache.emplace(curr_page_first_rec, cloned);
	assert (pair.second);
	curr_rs = rs;
}

int PaginalRS::getInt(const size_t field, bool &is_null) const {

	return curr_rs->getInt(field, is_null);
}

void PaginalRS::reload() {

	rs_rec_count->reload();
	rs_rec_count->gotoRecord(0);
	bool is_null;
	rec_count = rs_rec_count->getInt(0, is_null);

	if (rec_count <= curr_page_first_rec || curr_rs != rs) {
		curr_page_first_rec = rec_count % PAGE_SIZE == 0 ? \
							(rec_count >= PAGE_SIZE ? rec_count - PAGE_SIZE : 0) : \
							(rec_count / PAGE_SIZE) * PAGE_SIZE;

		stmt->bindValue(param_limit_size, PAGE_SIZE);
		stmt->bindValue(param_limit_size + 1, (int)curr_page_first_rec);
	}
	rs->reload();
	rs_cache.clear();

	addRSToCache();
}

const char *PaginalRS::getString(const size_t field) const {

	return curr_rs->getString(field);
}

const wchar_t *PaginalRS::getWString(const size_t field) const {

	return curr_rs->getWString(field);
}

ImmutableString<char> PaginalRS::getImmutableString(const size_t field) const {

	return curr_rs->getImmutableString(field);
}

ImmutableString<wchar_t> PaginalRS::getImmutableWString(const size_t field) const {

	return curr_rs->getImmutableWString(field);
}

CDate PaginalRS::getDate(const size_t field, bool &is_null) const {

	return curr_rs->getDate(field, is_null);
}


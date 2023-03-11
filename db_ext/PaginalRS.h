#pragma once
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>

class PaginalRS final {
	static constexpr size_t PAGE_SIZE = 100;
	size_t param_limit_size; // LIMIT row_count OFFSET offset_in_rs

	mutable std::shared_ptr<IDbStatement> stmt;
	std::shared_ptr<IDbStatement> stmt_rec_count;
	size_t rec_count;
	mutable size_t curr_page;
	mutable std::shared_ptr<IDbResultSet> rs;

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
	if (curr_page > record || record > curr_page + PAGE_SIZE) {

		curr_page = (record / PAGE_SIZE) * PAGE_SIZE;
		stmt->bindValue(param_limit_size, (int)PAGE_SIZE);
		stmt->bindValue(param_limit_size + 1, (int)curr_page);

		rs->reload();
	}

	rs->gotoRecord(record % PAGE_SIZE);
}

int PaginalRS::getInt(const size_t field, bool &is_null) const {

	return rs->getInt(field, is_null);
}

void PaginalRS::reload() {

	rec_count = stmt_rec_count->execScalar();

	if (rec_count <= curr_page) {
		size_t record = rec_count >= PAGE_SIZE / 2 ? \
						rec_count - PAGE_SIZE / 2 : rec_count / 2;

		curr_page = (record / PAGE_SIZE) * PAGE_SIZE;
		stmt->bindValue(param_limit_size, PAGE_SIZE);
		stmt->bindValue(param_limit_size + 1, (int)curr_page);
	}
	rs->reload();
}

const char *PaginalRS::getString(const size_t field) const {

	return rs->getString(field);
}

const wchar_t *PaginalRS::getWString(const size_t field) const {

	return rs->getWString(field);
}

ImmutableString<char> PaginalRS::getImmutableString(const size_t field) const {

	return rs->getImmutableString(field);
}

ImmutableString<wchar_t> PaginalRS::getImmutableWString(const size_t field) const {

	return rs->getImmutableWString(field);
}

CDate PaginalRS::getDate(const size_t field, bool &is_null) const {

	return rs->getDate(field, is_null);
}


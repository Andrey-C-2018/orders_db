#pragma once
#include <unordered_map>
#include <db/IDbStatement.h>
#include <db/IDbResultSet.h>
#include "StaticResultSet.h"

class PaginalRS : public IDbResultSet {
	static constexpr size_t PAGE_SIZE = 100;
	size_t param_limit_size; // LIMIT row_count OFFSET offset_in_rs
	size_t field_id_index;

	mutable std::shared_ptr<IDbStatement> stmt;
	std::shared_ptr<IDbStatement> stmt_rec_count;
	size_t rec_count;

	mutable size_t curr_page_first_rec, last_binded_page;
	mutable std::unordered_map<size_t, std::shared_ptr<StaticResultSet> > rs_cache;
	mutable std::shared_ptr<StaticResultSet> curr_rs;

	mutable std::shared_ptr<IDbResultSet> rs;
	std::shared_ptr<IDbResultSet> rs_rec_count;

	inline void addRSToCache() const;

public:
	PaginalRS(std::shared_ptr<IDbStatement> stmt_,
			  std::shared_ptr<IDbStatement> stmt_rec_count_,
			  size_t field_id_index_);

	PaginalRS(const PaginalRS &obj) = delete;
	PaginalRS(PaginalRS &&obj) = default;
	PaginalRS& operator=(const PaginalRS &obj) = delete;
	PaginalRS& operator=(PaginalRS &&obj) = default;

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
	void upload(IDbStaticResultSet &obj) const override { };

	virtual ~PaginalRS();
};

//*****************************************************

void PaginalRS::addRSToCache() const {

	auto new_cached_rs = std::make_shared<StaticResultSet>();
	rs->upload(*new_cached_rs);

	curr_rs = new_cached_rs;
	auto pair = rs_cache.emplace(curr_page_first_rec, new_cached_rs);
	assert (pair.second);
}
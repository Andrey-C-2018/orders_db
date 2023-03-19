#pragma once
#include <memory>
#include <db/IDbResultSet.h>

class CRevDbResultSet : public IDbResultSet {
	std::shared_ptr<IDbResultSet> actual_rs;
	bool rev_order;
	size_t records_count;

public:
	CRevDbResultSet(std::shared_ptr<IDbResultSet> actual_rs_, \
					const bool rev_order_);

	CRevDbResultSet(const CRevDbResultSet &obj) = delete;
	CRevDbResultSet(CRevDbResultSet &&obj) = default;
	CRevDbResultSet &operator=(const CRevDbResultSet &obj) = delete;
	CRevDbResultSet &operator=(CRevDbResultSet &&obj) = default;

	inline void setSortingOrder(const bool order) { rev_order = order; }

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

	virtual ~CRevDbResultSet();
};


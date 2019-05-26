#pragma once
#include <basic/ImmutableString.h>
#include <assert.h>

class CQueryModifier {
	std::string query;
	size_t where_pos;
	size_t where_size;
	bool where_present;

	inline void InternalInit();
public:
	CQueryModifier() noexcept;
	CQueryModifier(std::string query_);
	void Init(std::string query_);

	CQueryModifier(const CQueryModifier &obj) = default;
	CQueryModifier(CQueryModifier &&obj) = default;
	CQueryModifier &operator=(const CQueryModifier &obj) = default;
	CQueryModifier &operator=(CQueryModifier &&obj) = default;

	inline const std::string &getQuery() const { return query; }
	void changeWherePart(ImmutableString<char> where_part);
	void changeOrdering(const size_t field_to_order);

	virtual ~CQueryModifier();
};

void CQueryModifier::InternalInit() {

	const char where_subst_token[] = " ####";

	where_pos = query.rfind(where_subst_token, query.size());
	assert(where_pos != std::string::npos);

	query.erase(where_pos, sizeof(where_subst_token) - 1);
	where_size = 0;
}
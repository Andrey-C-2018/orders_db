#pragma once
#include <basic/ImmutableString.h>
#include <assert.h>

class CQueryModifierException : public XException {
public:
	enum {
		E_NO_WHERE_STMT_MARK = 1
	};
	CQueryModifierException(const int err_code, const Tchar *err_descr);
	CQueryModifierException(const CQueryModifierException &obj);
	CQueryModifierException(CQueryModifierException &&obj) = default;
	virtual ~CQueryModifierException();
};

class CQueryModifier {
public:
	struct QueryPart final {
		size_t pos;
		size_t size;
	};

private:
	std::string query;
	QueryPart where_part, order_by_part;

	inline void InternalInit(const size_t where_op_place);

public:
	CQueryModifier() noexcept;
	CQueryModifier(std::string query_);
	CQueryModifier(std::string query_, const size_t where_op_place);
	void Init(std::string query_);
	void Init(std::string query_, const size_t where_op_place);

	CQueryModifier(const CQueryModifier &obj) = default;
	CQueryModifier(CQueryModifier &&obj) = default;
	CQueryModifier &operator=(const CQueryModifier &obj) = default;
	CQueryModifier &operator=(CQueryModifier &&obj) = default;

	inline const std::string &getQuery() const { return query; }
	void changeWherePart(ImmutableString<char> where_part_str);
	void changeOrdering(ImmutableString<char> order_part_str);
	void resetWherePart();
	void resetOrdering();
	
	virtual ~CQueryModifier();
};

//*****************************************************

void CQueryModifier::InternalInit(const size_t where_op_place) {

	assert(where_op_place <= query.size());
	where_part.pos = where_op_place;
	where_part.size = 0;

	auto p = query.find(" ORDER BY");
	if (p == std::string::npos) {
		order_by_part.pos = 0;
		order_by_part.size = 0;
	}
	else {
		order_by_part.pos = p;
		order_by_part.size = query.size() - p;
	}
}
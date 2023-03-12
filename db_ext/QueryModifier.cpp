#include <basic/Exception.h>
#include "QueryModifier.h"

CQueryModifierException::CQueryModifierException(const int err_code, \
											const Tchar *err_descr) : \
											XException(err_code, err_descr) { }

CQueryModifierException::CQueryModifierException(const CQueryModifierException &obj) : \
											XException(obj) { }

CQueryModifierException::~CQueryModifierException() { }

//*************************************************************

CQueryModifier::CQueryModifier() noexcept { }

CQueryModifier::CQueryModifier(std::string query_) {

	Init(std::move(query_));
}

CQueryModifier::CQueryModifier(std::string query_, \
								const size_t where_op_place) : \
						query(std::move(query_)) {

	InternalInit(where_op_place);
}

void CQueryModifier::Init(std::string query_) {

	const char mark[] = " ####";

	auto p = query_.find(mark);
	if (p == std::string::npos) {
		CQueryModifierException e(CQueryModifierException::E_NO_WHERE_STMT_MARK, \
								_T("No WHERE stmt mark in the scalar_query_cache: "));
		e << query_;
		throw e;
	}

	query = std::move(query_);
	size_t sz = query.size();
	query.erase(p, sizeof(mark) - 1);

	InternalInit(p);
}

void CQueryModifier::Init(std::string query_, \
							const size_t where_op_place) {

	query = std::move(query_);
	InternalInit(where_op_place);
}

void CQueryModifier::changeWherePart(ImmutableString<char> where_part_str) {

	assert(where_part.pos);
	assert(!where_part_str.isNull());
	size_t sz = sizeof(" WHERE ") - 1;

	if (!where_part.size) {

		size_t old_size = query.size();
		query.insert(where_part.pos, " WHERE ", sz);
		query.insert(where_part.pos + sz, \
						where_part_str.str, where_part_str.size);
		where_part.size = query.size() - old_size;
		order_by_part.pos += where_part.size;
	}
	else {
		
		query.replace(where_part.pos + sz, where_part.size - sz, \
						where_part_str.str, where_part_str.size);

		where_part.size = sz + where_part_str.size;
		order_by_part.pos = where_part.pos + where_part.size;
	}
}

void CQueryModifier::changeOrdering(ImmutableString<char> order_part_str) {

	assert(where_part.pos);
	assert(!order_part_str.isNull());
	if (!order_by_part.size) {

		order_by_part.pos = query.size();
		query += " ORDER BY ";
		query.append(order_part_str.str, order_part_str.size);
		order_by_part.size = query.size() - order_by_part.pos;
	}
	else {
		size_t sz = sizeof(" ORDER BY ") - 1;
		query.replace(order_by_part.pos + sz, order_by_part.size - sz, \
						order_part_str.str, order_part_str.size);
		order_by_part.size += order_part_str.size;
	}
}

void CQueryModifier::resetWherePart() {

	query.erase(where_part.pos, where_part.size);
	order_by_part.pos -= where_part.size;
	where_part.size = 0;
}

void CQueryModifier::resetOrdering() {

	query.erase(order_by_part.pos, order_by_part.size);
	order_by_part.size = 0;
}

CQueryModifier::~CQueryModifier(){ }

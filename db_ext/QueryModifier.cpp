#include "QueryModifier.h"

CQueryModifier::CQueryModifier() noexcept : where_pos(std::string::npos), \
											where_size(0), where_present(false) { }

CQueryModifier::CQueryModifier(std::string query_) : query(std::move(query_)), \
														where_pos(std::string::npos), \
														where_size(0), where_present(false) {
	InternalInit();
}

void CQueryModifier::Init(std::string query_) {

	query = std::move(query_);
	InternalInit();
}

void CQueryModifier::changeWherePart(ImmutableString<char> where_part) {

	assert(where_part.str);

	if (where_part.size){
		if (!where_present) {
			query.insert(where_pos, " WHERE ");
			where_pos += sizeof(" WHERE ") - 1;
			where_size = 0;
			where_present = true;
		}
	}
	else {
		if (where_present) {
			query.erase(where_pos - sizeof(" WHERE ") + 1, \
						where_size + sizeof(" WHERE ") - 1);
			where_pos -= sizeof(" WHERE ") - 1;
			where_size = 0;
			where_present = false;
		}
	}
	query.replace(where_pos, where_size, where_part.str, where_part.size);
	where_size = where_part.size;
}

void CQueryModifier::changeOrdering(const size_t field_to_order) { }

CQueryModifier::~CQueryModifier(){ }

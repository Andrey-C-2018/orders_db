#include "UniqueIdRS.h"

UniqueIdRS::UniqueIdRS(CumulativeRS rs_, int field_id_index_) : \
		field_id_index(field_id_index_), not_unique(false), \
		rs(std::move(rs_) {
	
	
}
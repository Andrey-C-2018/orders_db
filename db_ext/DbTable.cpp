#include "DbTable.h"

class CBindValuePredicate final{
	std::shared_ptr<const CDbResultSet> result_set;
	size_t curr_record;
public:
	inline void operator()(const size_t field, \
				std::shared_ptr<IDbBindingTarget> binding_target) const{

		result_set->getValueAndBindItTo( binding_target );
	}
};

//****************************************************

CDbTable::CDbTable(){ }

void CDbTable::SetCell(const size_t field, const size_t record, \
							const Tchar *value) { }

CDbTable::~CDbTable(){ }

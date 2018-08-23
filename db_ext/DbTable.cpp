#include "DbResultSet.h"
#include "MetaInfo.h"

class CGetCellPredicate : public std::unary_function<CDbFieldValue, void> {
	const CDbTable &table;
public:
	CGetCellPredicate(const CDbResultSet &result_set_) : result_set(result_set_) { }
	inline void operator()(CDbFieldValue &field_value) {

		return table.GetDbCellValue(field_value);
	}
};

//****************************************************************************

CDbTable::CDbTable(){ }

void CDbTable::SetCell(const size_t field, const size_t record, \
							const Tchar *value) {


}

CDbTable::~CDbTable(){ }

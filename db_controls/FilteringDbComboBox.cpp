#include "FilteringDbComboBox.h"
#include <db_ext/FilteringManager.h>

CFilteringDbComboBox::CFilteringDbComboBox(CFilteringManager &filtering_manager_, \
								std::shared_ptr<const IDbResultSet> result_set, \
								const size_t field_to_display_, const size_t prim_key_) : \
	CDbComboBox(result_set, field_to_display_, prim_key_), \
	filtering_manager(filtering_manager_), id_expr(-1) { }

void CFilteringDbComboBox::fillComboBox() {

	addEmptyValue();
	CDbComboBox::fillComboBox();
}

void CFilteringDbComboBox::OnItemChoosed() {

	if (id_expr != (size_t)-1) {
		if (!isEmptySelection())
			filtering_manager.enableExpr(id_expr);
		else
			filtering_manager.disableExpr(id_expr);
	}
}

CFilteringDbComboBox::~CFilteringDbComboBox() { }

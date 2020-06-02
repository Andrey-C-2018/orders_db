#include <db/IDbBindingTarget.h>
#include <db_ext/IBinder.h>
#include <db_ext/FilteringManager.h>
#include "FilteringComboBox.h"

CFilteringComboBox::CFilteringComboBox(CFilteringManager &filtering_manager_) : \
					sel_index((size_t)-1), filtering_manager(filtering_manager_), \
					id_expr(-1) { }

void CFilteringComboBox::Create(XWindow *parent, const int flags, \
						const Tchar *label, \
						const int x, const int y, \
						const int width, const int height) {

	XComboBox::Create(parent, flags | FL_COMBOBOX_DROPDOWN, label, x, y, width, height);
	
	fill();
	parent->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
					GetId(), this, &CFilteringComboBox::OnItemChoosed);
}

void CFilteringComboBox::OnItemChoosed(XCommandEvent *eve) {

	sel_index = GetCurrentSelectionIndex();
	assert(id_expr != (size_t)-1);

	if (!isEmptySelection())
		filtering_manager.enableExpr(id_expr);
	else
		filtering_manager.disableExpr(id_expr);
}

CFilteringComboBox::~CFilteringComboBox() { }

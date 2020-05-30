#include "FilteringDateField.h"
#include <db_ext/FilteringManager.h>

CFilteringDateField::CFilteringDateField(CFilteringManager &filtering_manager_, \
											ITabStopManager *manager) : \
							XDateField(manager), \
							is_changed(false), id_expr(-1), \
							filtering_manager(filtering_manager_) { } 

void CFilteringDateField::Create(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {

	assert(!IsCreated());
	assert(id_expr != -1);

	XDateField::Create(parent, flags | FL_WINDOW_BORDERED | \
						FL_WINDOW_CLIPSIBLINGS, \
						label, x, y, width, height);

	parent->Connect(EVT_COMMAND, NCODE_EDIT_LOOSE_FOCUS, \
					GetId(), this, &CFilteringDateField::OnLooseFocus);
}

void CFilteringDateField::OnChange() {

	is_changed = true;
}

void CFilteringDateField::OnLooseFocus(XCommandEvent *eve) {

	if (is_changed) {
		const Tchar *label = eve->GetSender()->GetLabel();
		if (label && label[0] != _T('\0'))
			filtering_manager.enableExpr(id_expr);
		else
			filtering_manager.disableExpr(id_expr);

		is_changed = false;
	}
}

CFilteringDateField::~CFilteringDateField() { }

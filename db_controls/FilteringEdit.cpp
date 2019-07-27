#include "FilteringEdit.h"
#include <db_ext/FilteringManager.h>

CFilteringEdit::CFilteringEdit(CFilteringManager &filtering_manager_, \
								ITabStopManager *manager) : \
							XTabStopWidget<XEdit>(manager), \
							is_changed(false), id_expr(-1), \
							filtering_manager(filtering_manager_) { }

void CFilteringEdit::Create(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) {

	assert(!IsCreated());
	assert(id_expr != -1);

	XTabStopWidget<XEdit>::Create(parent, flags | FL_WINDOW_BORDERED | \
												FL_WINDOW_CLIPSIBLINGS | \
												FL_EDIT_AUTOHSCROLL, \
									label, x, y, width, height);
	parent->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
					GetId(), this, &CFilteringEdit::OnChange);
	parent->Connect(EVT_COMMAND, NCODE_EDIT_LOOSE_FOCUS, \
					GetId(), this, &CFilteringEdit::OnLooseFocus);
}

void CFilteringEdit::OnChange(XCommandEvent *eve) {

	is_changed = true;
}

void CFilteringEdit::OnLooseFocus(XCommandEvent *eve) {

	if (is_changed) {
		int id_expr = eve->GetSender()->GetTag();

		const Tchar *label = eve->GetSender()->GetLabel();
		if (label && label[0] != _T('\0'))
			filtering_manager.enableExpr(id_expr);
		else
			filtering_manager.disableExpr(id_expr);

		is_changed = false;
	}
}

CFilteringEdit::~CFilteringEdit() { }
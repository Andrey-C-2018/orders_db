#include "XTabStopEdit.h"

XTabStopEdit::XTabStopEdit(ITabStopManager *manager_) noexcept : \
						XTabStopWidget<XEdit>(manager_), edit_action(this) {

	setTabStopAction(&edit_action);
}

XTabStopEdit::XTabStopEdit(ITabStopManager *manager_, XWindow *parent, \
							const int flags, const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) : \
						XTabStopWidget<XEdit>(manager_), edit_action(this) {

	setTabStopAction(&edit_action);
	Create(parent, flags, label, x, y, width, height);
}

XTabStopEdit::~XTabStopEdit() { }
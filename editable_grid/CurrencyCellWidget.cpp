#include "CurrencyCellWidget.h"

CCurrencyCellWidget::CCurrencyCellWidget() : currency_filter(this, _T(','), _T('.')) { }

void CCurrencyCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
								FL_WINDOW_BORDERED, label, x, y, width, height);
}

void CCurrencyCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	this->on_change_caller = on_change.getDelegate();
	this->args_container = on_change.getArgsContainer();
	this->eve = on_change.getEventObj();

	XEdit::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
								XEdit::GetId(), this, &CCurrencyCellWidget::OnChange);
}

void CCurrencyCellWidget::OnChange(XCommandEvent *eve) {

	if (currency_filter.OnChange(eve))
		on_change_caller.Call(args_container.get());
}

CCurrencyCellWidget::~CCurrencyCellWidget() { }

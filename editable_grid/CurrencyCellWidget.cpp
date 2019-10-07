#include "CurrencyCellWidget.h"

CCurrencyCellWidget::CCurrencyCellWidget(const bool null_allowed_) : \
				currency_filter(this, _T(','), _T('.'), null_allowed_, _T("0.0")) { }

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

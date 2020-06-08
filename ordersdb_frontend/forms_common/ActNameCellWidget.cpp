#include "ActNameCellWidget.h"

CActNameCellWidget::CActNameCellWidget() : validator(this), ignore_change_event(false) { }

void CActNameCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	this->on_change_caller = on_change.getDelegate();
	this->args_container = on_change.getArgsContainer();
	this->eve = on_change.getEventObj();

	XEdit::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
								XEdit::GetId(), this, &CActNameCellWidget::OnChange);
}

void CActNameCellWidget::OnChange(XCommandEvent *eve) {

	if (!ignore_change_event)
		on_change_caller.Call(args_container.get());
}

bool CActNameCellWidget::Validate() const {

	auto label = XEdit::GetLabel(label_cache);
	ImmutableString<Tchar> act_name(label, label_cache.size());

	return InternalValidate(act_name);
}

bool CActNameCellWidget::Validate(ImmutableString<Tchar> act_name) const {

	return InternalValidate(act_name);
}

CActNameCellWidget::~CActNameCellWidget() { }

#include "DateCellWidget.h"

CDateCellWidget::CDateCellWidget() : date_filter(this, _T(','), _T('.')), \
									null_value_allowed(false) { }

CDateCellWidget::CDateCellWidget(const bool null_value_allowed_) : \
									date_filter(this, _T(','), _T('.')), \
									null_value_allowed(null_value_allowed_) { }

void CDateCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
					FL_WINDOW_BORDERED, label, x, y, width, height);
	XEdit::SetEditMaxLength(CDate::GERMAN_FORMAT_LEN);
}

void CDateCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	this->on_change_caller = on_change.getDelegate();
	this->args_container = on_change.getArgsContainer();
	this->eve = on_change.getEventObj();

	XEdit::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
								XEdit::GetId(), this, &CDateCellWidget::OnChange);
}

void CDateCellWidget::OnChange(XCommandEvent *eve) {

	if (date_filter.OnChange(eve)) 
		on_change_caller.Call(args_container.get());
}

bool CDateCellWidget::Validate() const {

	return InternalValidate(date_filter.getCachedLabel());
}

bool CDateCellWidget::Validate(ImmutableString<Tchar> label) const {

	return InternalValidate(label);
}

CDateCellWidget::~CDateCellWidget() { }

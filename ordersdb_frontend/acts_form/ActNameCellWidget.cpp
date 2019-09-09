#include "ActNameCellWidget.h"

CActNameCellWidget::CActNameCellWidget() : validator(this), ignore_change_event(false) { }

void CActNameCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
										const Tchar *label, \
										const int x, const int y, \
										const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL | \
					FL_WINDOW_BORDERED, label, x, y, width, height);
}

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

	Tstring error_str;
	bool cancel = !validator.validate(act_name, error_str);
	if (cancel || !error_str.empty()) {
		ErrorBox(error_str.c_str());
		return false;
	}

	validator.refreshActNameWidget(ignore_change_event);
	return true;
}

bool CActNameCellWidget::Validate(ImmutableString<Tchar> act_name) const {

	Tstring error_str;
	bool cancel = !validator.validate(act_name, error_str);
	if (cancel || !error_str.empty()) {
		ErrorBox(error_str.c_str());
		return false;
	}

	validator.refreshActNameWidget(ignore_change_event);
	return true;
}

CActNameCellWidget::~CActNameCellWidget() { }

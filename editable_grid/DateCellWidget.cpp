#include "DateCellWidget.h"
#include <date/Date.h>

CDateCellWidget::CDateCellWidget() : date_filter(this, _T(','), _T('.')) { }

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

	auto validated_label = date_filter.getCachedLabel();
	CDate dt(validated_label.str, CDate::GERMAN_FORMAT);

	if (dt.isNull()) {
		Tstring err_str = _T("Невірний формат дати: '");
		err_str += validated_label.str;
		err_str += _T("'");
		ErrorBox(err_str.c_str());
	}

	return !dt.isNull();
}

bool CDateCellWidget::Validate(ImmutableString<Tchar> label) const {

	CDate dt(label.str, CDate::GERMAN_FORMAT);

	if (dt.isNull()) {
		Tstring err_str = _T("Невірний формат дати: '");
		err_str += label.str;
		err_str += _T("'");
		ErrorBox(err_str.c_str());
	}

	return !dt.isNull();
}

CDateCellWidget::~CDateCellWidget() { }

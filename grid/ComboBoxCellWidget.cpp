#include "ComboBoxCellWidget.h"

CComboBoxCellWidget::CComboBoxCellWidget() { }

CComboBoxCellWidget::CComboBoxCellWidget() { }

void CComboBoxCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
	const Tchar *label, \
	const int x, const int y, \
	const int width, const int height) {

	assert(!IsCreated());
	XComboBox::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL, \
						label, x, y, width, height);
}

void CComboBoxCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
		XComboBox::GetId(), std::move(on_change));
}

void CComboBoxCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_LOOSE_FOCUS, \
		XComboBox::GetId(), std::move(on_loose_focus));
}

void CComboBoxCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

	OverrideWindowEvent(EVT_KEYDOWN, std::move(on_key_press));
}

void CComboBoxCellWidget::SetCurrentField(const size_t field) { }

void CComboBoxCellWidget::Show() {

	XComboBox::Show();
}

void CComboBoxCellWidget::Hide() {

	XComboBox::Hide();
}

void CComboBoxCellWidget::MoveWindow(const int x, const int y, \
	const int width, const int height) {

	XComboBox::MoveWindow(x, y, width, height);
}

void CComboBoxCellWidget::SetFocus() {

	XComboBox::SetFocus();
}

ImmutableString<Tchar> CComboBoxCellWidget::GetLabel() {
	size_t size(0);

	const Tchar *label = XComboBox::GetLabel(size);
	return ImmutableString<Tchar>(label, size);
}

void CComboBoxCellWidget::SetLabel(ImmutableString<Tchar> label) {

	XComboBox::SetLabel(label.str, label.size);
}

int CComboBoxCellWidget::GetId() const {

	return XComboBox::GetId();
}

CComboBoxCellWidget::~CComboBoxCellWidget()
{
}

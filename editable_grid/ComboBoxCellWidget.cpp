#include "ComboBoxCellWidget.h"

CComboBoxCellWidget::CComboBoxCellWidget() { }

void CComboBoxCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	assert(!IsCreated());
	XComboBox::Create(parent, flags | FL_COMBOBOX_DROPDOWN,
						label, x, y, width, height + DROPDOWN_PART_SIZE);
}

void CComboBoxCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
									XComboBox::GetId(), std::move(on_change));
}

void CComboBoxCellWidget::SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> ignored) { }

void CComboBoxCellWidget::SetOnGetFocusHandler(XEventHandlerData on_get_focus) {

	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_COMBOBOX_GET_FOCUS, \
									XComboBox::GetId(), std::move(on_get_focus));
	
}

void CComboBoxCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_COMBOBOX_LOOSE_FOCUS, \
									XComboBox::GetId(), std::move(on_loose_focus));
}

void CComboBoxCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

	XComboBox::OverrideWindowEvent(EVT_KEYDOWN, std::move(on_key_press));
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

	XComboBox::MoveWindow(x, y, width, height + DROPDOWN_PART_SIZE);
}

void CComboBoxCellWidget::SetFocus() {

	XComboBox::SetFocus();
}

bool CComboBoxCellWidget::HasFocus() const {

	return XComboBox::ComboBoxHasFocus();
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

CComboBoxCellWidget::~CComboBoxCellWidget(){ }

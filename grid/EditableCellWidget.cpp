#include "EditableCellWidget.h"

IMPLEMENT_EVENT_OVERRIDE(CEditableCellWidget)

CEditableCellWidget::CEditableCellWidget() { }

void CEditableCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
								const Tchar *label, \
								const int x, const int y, \
								const int width, const int height) {

	assert(!IsCreated());
	XEdit::Create(parent, flags | FL_WINDOW_CLIPSIBLINGS | FL_EDIT_AUTOHSCROLL, \
					label, x, y, width, height);
}

void CEditableCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

	XEdit::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_CHANGED, \
								XEdit::GetId(), std::move(on_change));
}

void CEditableCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

	XEdit::GetParent()->Connect(EVT_COMMAND, NCODE_EDIT_LOOSE_FOCUS, \
								XEdit::GetId(), std::move(on_loose_focus));
}

void CEditableCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

	OverrideWindowEvent(EVT_KEYDOWN, std::move(on_key_press));
}

void CEditableCellWidget::SetCurrentField(const size_t field) { }

void CEditableCellWidget::Show() {

	XEdit::Show();
}

void CEditableCellWidget::Hide() {

	XEdit::Hide();
}

void CEditableCellWidget::MoveWindow(const int x, const int y, \
										const int width, const int height) {

	XEdit::MoveWindow(x, y, width, height);
}

void CEditableCellWidget::SetFocus() {

	XEdit::SetFocus();
}

ImmutableString<Tchar> CEditableCellWidget::GetLabel() {
	size_t size(0);

	const Tchar *label = XEdit::GetLabel(size);
	return ImmutableString<Tchar>(label, size);
}

void CEditableCellWidget::SetLabel(ImmutableString<Tchar> label) {

	XEdit::SetLabel(label.str, label.size);
}

int CEditableCellWidget::GetId() const {

	return XEdit::GetId();
}

CEditableCellWidget::~CEditableCellWidget() { }

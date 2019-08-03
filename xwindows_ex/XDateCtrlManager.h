#pragma once
#include <date/Date.h>
#include <xwindows/XEdit.h>

class XDateCtrlManager {
	XEdit *edit;
	std::vector<Tchar> label;
	bool ignore_change_event;
public:
	XDateCtrlManager(XEdit *edit_) : edit(edit_), ignore_change_event(false) { }

	XDateCtrlManager(XDateCtrlManager &&obj) = default;
	XDateCtrlManager(const XDateCtrlManager &obj) = delete;
	XDateCtrlManager &operator=(const XDateCtrlManager &obj) = delete;
	XDateCtrlManager &operator=(XDateCtrlManager &&obj) = default;

	inline bool OnChange(XEvent *eve);
	virtual ~XDateCtrlManager() { }
};

//****************************************************************************

bool XDateCtrlManager::OnChange(XEvent *eve) {

	if (ignore_change_event) return false;

	edit->GetLabel(label);

	auto p = Tstrchr(&label[0], _T(','));
	if (p) {
		size_t index = p - &label[0];

		label[index] = _T('.');

		ignore_change_event = true;
		edit->SetLabel(&label[0], label.size());
		edit->SetSelection(index + 1, index + 1);
		ignore_change_event = false;
	}

	return true;
}
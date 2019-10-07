#pragma once
#include <basic/ImmutableString.h>
#include <xwindows/XEdit.h>

class XCtrlInputFilter {
	XEdit *edit;
	std::vector<Tchar> label;
	bool ignore_change_event, empty_allowed;
	Tchar what_to_search, replacement;
	const Tchar *empty_replacement;
public:
	XCtrlInputFilter(XEdit *edit_, const Tchar what_to_search_, \
						const Tchar replacement_, \
						const bool empty_allowed_, \
						const Tchar *empty_replacement_) : \
					edit(edit_), ignore_change_event(false), \
					what_to_search(what_to_search_), replacement(replacement_), \
					empty_allowed(empty_allowed_), empty_replacement(empty_replacement_) { }

	XCtrlInputFilter(XCtrlInputFilter &&obj) = default;
	XCtrlInputFilter(const XCtrlInputFilter &obj) = delete;
	XCtrlInputFilter &operator=(const XCtrlInputFilter &obj) = delete;
	XCtrlInputFilter &operator=(XCtrlInputFilter &&obj) = default;

	inline bool OnChange(XEvent *eve);
	inline ImmutableString<Tchar> getCachedLabel() const;

	virtual ~XCtrlInputFilter() { }
};

//****************************************************************************

bool XCtrlInputFilter::OnChange(XEvent *eve) {

	if (ignore_change_event) return false;

	edit->GetLabel(label);
	if (!empty_allowed && (label.empty() || (!label.empty() && label[0] == _T('\0')))) {
		if (empty_replacement) {
			ignore_change_event = true;
			edit->SetLabel(empty_replacement);
			ignore_change_event = false;
			return true;
		}
		return false;
	}
	
	auto p = Tstrchr(&label[0], what_to_search);
	if (p) {
		size_t index = p - &label[0];

		label[index] = replacement;

		ignore_change_event = true;
		edit->SetLabel(&label[0], label.size());
		edit->SetSelection(index + 1, index + 1);
		ignore_change_event = false;
	}

	return true;
}

ImmutableString<Tchar> XCtrlInputFilter::getCachedLabel() const {

	return ImmutableString<Tchar>(&label[0], label.size());
}
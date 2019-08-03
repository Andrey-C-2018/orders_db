#include "XEdit.h"

XEdit::XEdit() : XWidget(){

	CheckAndInitWindowClassName(_T("EDIT"));
}

XEdit::XEdit(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height){

	CheckAndInitWindowClassName(_T("EDIT"));
	Create(parent, flags, label, x, y, width, height);
}

void XEdit::SetEditMaxLength(const size_t max_len) {

	_plSetEditMaxLength(GetInternalHandle(), max_len);
}

void XEdit::SetSelection(const size_t begin, const size_t end) {

	assert(begin <= end);
	_plSetEditControlSelection(GetInternalHandle(), begin, end);
}

XEdit::~XEdit(){ }

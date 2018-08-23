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

XEdit::~XEdit(){ }

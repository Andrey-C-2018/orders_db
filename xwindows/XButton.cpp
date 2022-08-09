#include "XButton.h"

XButton::XButton(){

	CheckAndInitWindowClassName(_T("BUTTON"));
}

XButton::XButton(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height){

	CheckAndInitWindowClassName(_T("BUTTON"));
	Create(parent, flags, label, x, y, width, height);
}

XButton::~XButton(){ }
#include "XLabel.h"

XLabel::XLabel() : XWidget() {

CheckAndInitWindowClassName(_T("STATIC"));
}

XLabel::XLabel(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) {

CheckAndInitWindowClassName(_T("STATIC"));
Create(parent, flags, label, x, y, width, height);
}

XLabel::~XLabel(){ }

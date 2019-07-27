#include "XPanel.h"

bool XPanel::is_class_registered = false;

XPanel::XPanel() {

	if (!is_class_registered) {
		RegisterNewWidgetClass(_T("XPANEL"));
		is_class_registered = true;
	}
	else 
		CheckAndInitWindowClassName(_T("XPANEL"));
}

XPanel::XPanel(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height){

	if (!is_class_registered) {
		RegisterNewWidgetClass(_T("XPANEL"));
		is_class_registered = true;
	}
	else
		CheckAndInitWindowClassName(_T("XPANEL"));
	Create(parent, flags, label, x, y, width, height);
}

XPanel::~XPanel(){ }

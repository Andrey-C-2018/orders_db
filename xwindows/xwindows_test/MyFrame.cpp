#include "MyFrame.h"
#include <xwindows/XButton.h>
#include <xwindows/XEdit.h>
#include <xwindows/XLabel.h>
#include <xwindows/XComboBox.h>
#include <xwindows/XPanel.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) {
	
	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	Test();
}

void CMyFrame::Test() {

	auto panel = new XPanel(this, FL_WINDOW_VISIBLE, _T(""), 10, 10, 100, 100);
}

CMyFrame::~CMyFrame(){ }
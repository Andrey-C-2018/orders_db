#include "MyFrame.h"
#include <xwindows/XButton.h>
#include <xwindows/XEdit.h>
#include <xwindows/XLabel.h>
#include <xwindows/XComboBox.h>
#include "SimpleSizer.h"

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) {
	
	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	//Connect(EVT_SIZE, this, &CMyFrame::OnSize);

	XRect rc;
	GetClientRect(rc);

	InitTestControls(rc);
}

void CMyFrame::InitTestControls(const XRect &rc) {

	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, 40);

	main_sizer.addWidget(new XButton);
	main_sizer.addWidget(new XEdit);
	main_sizer.addWidget(new XButton);

	CHorizontalSizer sizer;
	main_sizer.initNestedSizer(sizer);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 60);

	main_sizer.addNestedSizer(sizer);
	main_sizer.addWidget(new XButton);
}

void CMyFrame::OnSize(XSizeEvent *eve) {


}

CMyFrame::~CMyFrame(){ }
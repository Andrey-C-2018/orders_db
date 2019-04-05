#include "MyFrame.h"
#include <xwindows/XButton.h>
#include <xwindows/XEdit.h>
#include <xwindows/XLabel.h>
#include <xwindows/XComboBox.h>
#include "VerticalSizer.h"
#include "HorizontalSizer.h"

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

	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, 40);

	main_sizer.addWidget(new XButton);
	main_sizer.addWidget(new XEdit);
	main_sizer.addWidget(new XButton);

	CHorizontalSizer sizer(0, 0, 0, 0);
	main_sizer.initNestedSizer(sizer);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 60);
	sizer.addWidget(new XButton(), 100, 20);

	CVerticalSizer sizer2(0, 0, 0, 0, 20);
	sizer.initNestedSizer(sizer2);
	sizer2.addWidget(new XButton(), _T("1"));
	sizer2.addWidget(new XButton(), _T("2"));

	sizer.addNestedSizerByActualSize(sizer2);

	main_sizer.addNestedSizerByActualSize(sizer);
	main_sizer.addWidget(new XButton);

	main_sizer.initNestedSizer(sizer);
	sizer.addWidget(new XButton(), 20, 20);
	sizer.initNestedSizerWithWidth(sizer2, 100);
	sizer2.addWidget(new XButton(), _T("1"));
	sizer2.addWidget(new XButton(), _T("2"));

	//sizer.addWidget(new XButton(), 20, 20);
}

void CMyFrame::OnSize(XSizeEvent *eve) {


}

CMyFrame::~CMyFrame(){ }
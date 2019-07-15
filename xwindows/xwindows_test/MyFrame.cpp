#include "MyFrame.h"
#include <xwindows/XButton.h>
#include <xwindows/XEdit.h>
#include <xwindows/XLabel.h>
#include <xwindows/XComboBox.h>
#include <xwindows/VerticalSizer.h>
#include <xwindows/HorizontalSizer.h>

CMyFrame::CMyFrame(const Tchar *class_name,\
					 const Tchar *label, const int X, const int Y,\
					 const int width, const int height) : sizer(10, 10), \
					sizer2(10, 10), sizer3(10, 10), sizer4(10, 10), sizer5(10, 10), \
					button1(nullptr), button2(nullptr), button3(nullptr), \
					button4(nullptr), button5(nullptr) {
	
	Create(class_name, FL_WINDOW_VISIBLE | FL_WINDOW_CLIPCHILDREN, \
			label, X, Y, width, height);

	XRect rc;
	GetClientRect(rc);

	//TestStaticSizers(rc);
	TestDynamicSizers(rc);
	Connect(EVT_SIZE, this, &CMyFrame::OnSize);
}

void CMyFrame::TestStaticSizers(const XRect &rc) {

	CVerticalSizer main_sizer(this, 0, 0, rc.right, rc.bottom, \
								10, 10, 10, 10, 2, 40);

	main_sizer.addWidget(new XButton);
	main_sizer.addWidget(new XEdit);
	main_sizer.addWidget(new XButton);

	CHorizontalSizer sizer(0, 0, 0, 0);
	main_sizer.pushNestedSizer(sizer);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 40);
	sizer.addWidget(new XButton(), 100, 60);
	sizer.addWidget(new XButton(), 100, 20);

	CSizerPreferences prefs(0, 0, 0, 0, 20);
	CVerticalSizer sizer2(prefs, 20);
	sizer.pushNestedSizer(sizer2);
	sizer2.addWidget(new XButton(), _T("1"));
	sizer2.addWidget(new XButton(), _T("2"));

	sizer.popNestedSizer();

	main_sizer.popNestedSizer();
	main_sizer.addWidget(new XButton);

	main_sizer.pushNestedSizer(sizer);
	sizer.addWidget(new XButton(), 20, 20);
	sizer.pushNestedSizer(sizer2, 100);
	sizer2.addWidget(new XButton(), _T("1"));
	sizer2.addWidget(new XButton(), _T("2"));
	sizer.popNestedSizer();

	sizer.addWidget(new XButton(), 20, 20);
	main_sizer.popNestedSizer();

	main_sizer.addLastWidget(new XButton());
}

void CMyFrame::TestDynamicSizers(const XRect &rc) {

	sizer.setCoords(XPoint(10, 10));
	sizer.setParentSize(XSize(rc.right, rc.bottom));
	sizer.setRelativeWidth(200);
	sizer.freezeWidth();
	sizer.setMinusY(40);
	button1 = new XButton();
	sizer.createWidget(button1, this, FL_WINDOW_VISIBLE, _T("1"));

	button2 = new XButton();
	sizer2.addToVertSizersChain(sizer);
	sizer2.createWidget(button2, this, FL_WINDOW_VISIBLE, _T("2"));

	button3 = new XButton();
	sizer3.addToHorzSizersChain(sizer);
	sizer3.setHeightMultiplier(0.5F);
	sizer3.createWidget(button3, this, FL_WINDOW_VISIBLE, _T("3"));

	button4 = new XButton();
	sizer4.addToVertSizersChain(sizer3);
	sizer4.setRelativeHeight(50);
	sizer4.freezeHeight();
	sizer4.createWidget(button4, this, FL_WINDOW_VISIBLE, _T("4"));

	button5 = new XButton();
	sizer5.addToVertSizersChain(sizer4);
	sizer5.createWidget(button5, this, FL_WINDOW_VISIBLE, _T("5"));
}

void CMyFrame::OnSize(XSizeEvent *eve) {

	if(!sizer.setParentSize(XSize(eve->GetWidth(), eve->GetHeight()))) return;
	sizer2.updateVertSizersChain(sizer);
	sizer3.updateHorzSizersChain(sizer);
	sizer4.updateVertSizersChain(sizer3);
	sizer5.updateVertSizersChain(sizer4);

	sizer.moveWidget(button1);
	sizer2.moveWidget(button2);
	sizer3.moveWidget(button3);
	sizer4.moveWidget(button4);
	sizer5.moveWidget(button5);
}

CMyFrame::~CMyFrame(){ }
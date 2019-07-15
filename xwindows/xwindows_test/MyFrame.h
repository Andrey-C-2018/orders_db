#pragma once
#include <xwindows/XFrame.h>
#include <xwindows/XDynamicSizer.h>

class XButton;

class CMyFrame : public XFrame{
	XDynamicSizer sizer, sizer2, sizer3, sizer4, sizer5;
	XButton *button1, *button2, *button3, *button4, *button5;

	int width, height;

	void TestStaticSizers(const XRect &rc);
	void TestDynamicSizers(const XRect &rc);
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	void OnSize(XSizeEvent *eve);
	virtual ~CMyFrame();
};

//****************************************************************************
#pragma once
#include <xwindows/XFrame.h>

class CSearchForm;

class CMyFrame : public XFrame{
	CSearchForm *main;
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	void OnSize(XSizeEvent *eve);
	virtual ~CMyFrame();
};

#pragma once
#include <xwindows/XFrame.h>

class CAdvocatsBook;

class CMyFrame : public XFrame{
	CAdvocatsBook *main;
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	void OnSize(XSizeEvent *eve);
	virtual ~CMyFrame();
};

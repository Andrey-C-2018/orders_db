#pragma once
#include <xwindows/XFrame.h>

class InformersBook;

class CMyFrame : public XFrame{
	InformersBook*main;
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	void OnSize(XSizeEvent *eve);
	virtual ~CMyFrame();
};

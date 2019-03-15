#pragma once
#include <xwindows/XFrame.h>

class CMyFrame : public XFrame{

public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	virtual ~CMyFrame();
};

//****************************************************************************

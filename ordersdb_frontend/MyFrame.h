#pragma once
#include <xwindows/XFrame.h>
#include <xwindows/XButton.h>
#include <xwindows/XButton.h>
#include <db_controls/DbGrid.h>

class CMyFrame : public XFrame{
	XButton *button_focus, *button_add, *button_remove, *button_refresh;
	CDbGrid *grid;

	std::shared_ptr<CDbTable> initDbTable();
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	virtual ~CMyFrame();
};

//****************************************************************************

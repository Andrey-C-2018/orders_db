#pragma once
#include <xwindows/XFrame.h>
#include <xwindows/XButton.h>
#include <xwindows/XButton.h>
#include <db_controls/DbGrid.h>

class CMyFrame : public XFrame{
	XButton *button_ce, *button_ce2;
	CDbGrid *grid;

	Tstring label;

public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);
	void OnSize(XSizeEvent *eve);
	void OnButtonNumberClick(XCommandEvent *eve);
	void OnButtonCeClick(XCommandEvent *eve);
	void OnButtonCe2Click(XCommandEvent *eve);
	virtual ~CMyFrame();
};

//****************************************************************************
#pragma once
#include <xwindows/XFrame.h>
#include <xwindows/XButton.h>
#include <xwindows/XEdit.h>
#include <xwindows/XLabel.h>
#include <xwindows/XComboBox.h>

class CMyFrame : public XFrame{
	XLabel *label1;
	XEdit *edit1;
	XButton *number_buttons[10];
	XButton *button_ce;
	XComboBox *actions;

	Tstring label;

	void InitTestControls(const XRect &rc);
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);
	void OnSize(XSizeEvent *eve);
	void OnButtonNumberClick(XCommandEvent *eve);
	void OnButtonCeClick(XCommandEvent *eve);
	virtual ~CMyFrame();
};

//****************************************************************************
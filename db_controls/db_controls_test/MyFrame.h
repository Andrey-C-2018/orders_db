#pragma once
#include <xwindows/XFrame.h>
#include <xwindows/XButton.h>
#include <db_controls/DbGrid.h>

class CMyFrame : public XFrame{
	CDbGrid *grid;
	XButton *button_focus, *button_add, *button_remove, *button_refresh;

	void OnSize(XSizeEvent *eve);
	void OnButtonFocusClick(XCommandEvent *eve);
	void OnButtonAddClick(XCommandEvent *eve);
	void OnButtonRemoveClick(XCommandEvent *eve);
	void OnButtonRefreshClick(XCommandEvent *eve);
public:
	CMyFrame(const Tchar *class_name,\
			 const Tchar *label, const int X, const int Y,\
			 const int width, const int height);

	virtual ~CMyFrame();
};

//****************************************************************************
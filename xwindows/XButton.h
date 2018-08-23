#pragma once
#include "XWidget.h"

class XButton : public XWidget{
	XButton(const XButton &obj) = delete;
	XButton &operator=(const XButton &obj) = delete;
public:
	XButton();
	XButton(XButton &&obj) = default;
	XButton &operator=(XButton &&obj) = default;
	XButton(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height);
	virtual ~XButton();
};

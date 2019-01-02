#pragma once
#include "XWidget.h"

class XEdit : public XWidget{
	XEdit(const XEdit &obj) = delete;
	XEdit &operator=(const XEdit &obj) = delete;
public:
	XEdit();
	XEdit(XEdit &&obj) = default;
	XEdit &operator=(XEdit &&obj) = default;

	XEdit(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height);
	virtual ~XEdit();
};
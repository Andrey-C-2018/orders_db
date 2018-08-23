#pragma once
#include "XWidget.h"

class XPanel :	public XWidget{
	static bool is_class_registered;

	XPanel(const XPanel& obj) = delete;
	XPanel &operator=(const XPanel& obj) = delete;
public:
	XPanel();
	XPanel(XPanel&& obj) = default;
	XPanel &operator=(XPanel&& obj) = default;
	XPanel(XWindow *parent, const int flags, \
			const Tchar *label, \
			const int x, const int y, \
			const int width, const int height);
	virtual ~XPanel();
};


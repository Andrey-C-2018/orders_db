#pragma once
#include "XWidget.h"

class XLabel : public XWidget {
public:
	XLabel();

	XLabel(const XLabel &obj) = delete;
	XLabel &operator=(const XLabel &obj) = delete;
	XLabel(XLabel &&obj) = default;
	XLabel &operator=(XLabel &&obj) = default;

	XLabel(XWindow *parent, const int flags, \
		const Tchar *label, \
		const int x, const int y, \
		const int width, const int height);

	virtual ~XLabel();
};


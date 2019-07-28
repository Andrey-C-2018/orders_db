#pragma once
#include <xwindows/XWidget.h>

class XNullWidget : public XWidget {
public:
	XNullWidget();

	XNullWidget(const XNullWidget &obj) = delete;
	XNullWidget(XNullWidget &&obj) = default;
	XNullWidget &operator=(const XNullWidget &obj) = delete;
	XNullWidget &operator=(XNullWidget &&obj) = default;

	XNullWidget(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	void Create(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	virtual ~XNullWidget();
};


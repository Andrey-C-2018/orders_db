#pragma once
#include "XWidget.h"

class XProgressBar : public XWidget {

	XProgressBar(const XProgressBar &obj) = delete;
	XProgressBar &operator=(const XProgressBar &obj) = delete;

public:
	XProgressBar();
	XProgressBar(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height);

	XProgressBar(XProgressBar &&obj) = default;
	XProgressBar &operator=(XProgressBar &&obj) = default;

	void SetParams(int range, int step);
	
	virtual ~XProgressBar();
};


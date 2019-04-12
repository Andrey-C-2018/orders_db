#pragma once
#include <xwindows/XWidget.h>
#include "Sizer.h"

class CHorizontalSizer : public CSizer {

	int x_initial;
	int max_widget_height;

	void onParametersChanged(const int new_x, const int new_y, \
							const int new_width, const int new_height) noexcept override;
public:
	CHorizontalSizer() noexcept;
	CHorizontalSizer(const int margin_left, const int margin_right, \
						const int margin_top, const int margin_bottom) noexcept;
	CHorizontalSizer(CSizerPreferences prefs) noexcept;
	CHorizontalSizer(XWindow *parent, \
						const int x, const int y, \
						const int width, const int height, \
						const int margin_left, const int margin_right, \
						const int margin_top, const int margin_bottom, \
						const int gap) noexcept;

	CHorizontalSizer(const CHorizontalSizer &obj) = default;
	CHorizontalSizer(CHorizontalSizer &&obj) = default;
	CHorizontalSizer &operator=(const CHorizontalSizer &obj) = default;
	CHorizontalSizer &operator=(CHorizontalSizer &&obj) = default;

	void addWidget(XWidget *widget, const Tchar *label, const int flags, XSize size);
	inline void addWidget(XWidget *widget, const Tchar *label, XSize size);
	inline void addWidget(XWidget *widget, const int widget_width, const int widget_height);

	void pushNestedSizer(CSizer &sizer);
	void pushNestedSizer(CSizer &sizer, const int width);
	void popNestedSizer();

	int getActualHeight() const override;

	bool isNull() const override;
	virtual ~CHorizontalSizer();
};

void CHorizontalSizer::addWidget(XWidget *widget, const Tchar *label, XSize size) {

	addWidget(widget, label, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, size);
}

void CHorizontalSizer::addWidget(XWidget *widget, const int widget_width, const int widget_height) {

	addWidget(widget, _T(""), FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
				XSize(widget_width, widget_height));
}
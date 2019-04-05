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
	CHorizontalSizer(XWindow *parent, \
						const int x, const int y, \
						const int width, const int height, \
						const int margin_left, const int margin_right, \
						const int margin_top, const int margin_bottom) noexcept;

	CHorizontalSizer(const CHorizontalSizer &obj) = default;
	CHorizontalSizer(CHorizontalSizer &&obj) = default;
	CHorizontalSizer &operator=(const CHorizontalSizer &obj) = default;
	CHorizontalSizer &operator=(CHorizontalSizer &&obj) = default;

	inline void addWidget(XWidget *widget, const int widget_width, const int widget_height);
	void addWidget(XWidget *widget, const Tchar *label, XSize size);
	void initNestedSizer(CSizer &sizer) const;
	void initNestedSizerWithWidth(CSizer &sizer, const int width) const;
	void addNestedSizerByActualSize(const CSizer &sizer);

	int getActualHeight() const override;

	bool isNull() const override;
	virtual ~CHorizontalSizer();
};

void CHorizontalSizer::addWidget(XWidget *widget, const int widget_width, const int widget_height) {

	addWidget(widget, _T(""), XSize(widget_width, widget_height));
}
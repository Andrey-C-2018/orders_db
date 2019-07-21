#pragma once
#include <xwindows/XWidget.h>
#include "Sizer.h"

class CVerticalSizer : public CSizer {

	int y_initial;
	int row_height;

	void onParametersChanged(const int new_x, const int new_y, \
							const int new_width, const int new_height) noexcept override;
public:
	CVerticalSizer(const int row_height) noexcept;
	CVerticalSizer(const int margin_left, const int margin_right, \
					const int margin_top, const int margin_bottom, \
					const int row_height) noexcept;
	CVerticalSizer(CSizerPreferences prefs, const int row_height) noexcept;
	CVerticalSizer(XWindow *parent, \
					const int x, const int y, \
					const int width, const int height, \
					const int margin_left, const int margin_right, \
					const int margin_top, const int margin_bottom, \
					const int gap, const int row_height) noexcept;

	CVerticalSizer(const CVerticalSizer &obj) = default;
	CVerticalSizer(CVerticalSizer &&obj) = default;
	CVerticalSizer &operator=(const CVerticalSizer &obj) = default;
	CVerticalSizer &operator=(CVerticalSizer &&obj) = default;

	void addWidget(XWidget *widget, const Tchar *label, const int flags);
	inline void addWidget(XWidget *widget, const Tchar *label);
	inline void addWidget(XWidget *widget);
	XRect addLastWidget(XWidget *widget);

	void pushNestedSizer(CSizer &sizer);
	void popNestedSizer();

	int getActualHeight() const override;
	inline XRect getMargins() const;

	inline bool isNull() const;
	virtual ~CVerticalSizer();
};

void CVerticalSizer::addWidget(XWidget *widget, const Tchar *label) {

	addWidget(widget, label, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE);
}

void CVerticalSizer::addWidget(XWidget *widget) {

	addWidget(widget, _T(""), FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE);
}

XRect CVerticalSizer::getMargins() const {

	return XRect(margin_left, margin_top, margin_right, margin_bottom);
}
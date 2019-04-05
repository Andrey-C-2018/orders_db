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
	CVerticalSizer(XWindow *parent, \
					const int x, const int y, \
					const int width, const int height, \
					const int margin_left, const int margin_right, \
					const int margin_top, const int margin_bottom, \
					const int row_height) noexcept;

	CVerticalSizer(const CVerticalSizer &obj) = default;
	CVerticalSizer(CVerticalSizer &&obj) = default;
	CVerticalSizer &operator=(const CVerticalSizer &obj) = default;
	CVerticalSizer &operator=(CVerticalSizer &&obj) = default;

	inline void addWidget(XWidget *widget);
	void addWidget(XWidget *widget, const Tchar *label);
	void initNestedSizer(CSizer &sizer) const;
	void addNestedSizerByActualSize(const CSizer &sizer);

	int getActualHeight() const override;

	inline bool isNull() const;
	virtual ~CVerticalSizer();
};

void CVerticalSizer::addWidget(XWidget *widget) {

	addWidget(widget, _T(""));
}
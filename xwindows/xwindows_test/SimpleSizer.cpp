#include <algorithm>
#include "SimpleSizer.h"

#undef max

CHorizontalSizer::CHorizontalSizer() noexcept : parent(nullptr), x_initial(0), x(0), y(0), \
										width(0), height(0), \
										max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(XWindow *parent_, \
				const int x_, const int y_, \
				const int width_, const int height_) noexcept : \
			parent(parent_), x_initial(x_), x(x_ + MARGIN_X), y(y_), \
			width(width_), height(height_), \
			max_widget_height(0) {

	assert(parent);
	assert(width > 0);
	assert(height > 0);
}

void CHorizontalSizer::addWidget(XWidget *widget, const int widget_width, const int widget_height) {

	assert(widget);
	assert(x - x_initial + widget_width + MARGIN_X <= width);
	assert(widget_height <= height);

	max_widget_height = std::max(max_widget_height, widget_height);
	
	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
					_T(""), x, y, widget_width, max_widget_height);
	x += widget_width + GAP_X;
}

void CHorizontalSizer::initNestedSizer(CVerticalSizer &sizer, const int row_height) const {

	sizer = CVerticalSizer(parent, x, y, width - MARGIN_X - x + x_initial, height, row_height);
}

void CHorizontalSizer::addNestedSizer(const CVerticalSizer &sizer) {
	
	assert(!sizer.isNull());
	assert(sizer.getHeight() <= height);

	max_widget_height = std::max(max_widget_height, sizer.getHeight());
	x = x_initial + width - MARGIN_X;
}

CHorizontalSizer::~CHorizontalSizer() { }

//****************************************************************************

CVerticalSizer::CVerticalSizer() noexcept : parent(nullptr), x(0), y_initial(0), y(0), \
									width(0), height(0), row_height(0) { }

CVerticalSizer::CVerticalSizer(XWindow *parent_, \
								const int x_, const int y_, \
								const int width_, const int height_, \
								const int row_height_) noexcept : \
						parent(parent_), x(x_), y_initial(y_), y(y_ + MARGIN_Y), \
						width(width_), height(height_), \
						row_height(row_height_) {

	assert(parent);
	assert(width > 0);
	assert(height > 0);
	assert(row_height <= height);
}

void CVerticalSizer::addWidget(XWidget *widget) {

	assert(widget);
	assert(y - y_initial + row_height + MARGIN_Y <= height);
	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
					_T(""), x, y, width, row_height);

	y += row_height + GAP_Y;
}

void CVerticalSizer::initNestedSizer(CHorizontalSizer &sizer) const {

	sizer = CHorizontalSizer(parent, x, y, width, height - MARGIN_Y - y + y_initial);
}

void CVerticalSizer::addNestedSizer(const CHorizontalSizer &sizer) {

	assert(!sizer.isNull());
	assert(sizer.getWidth() <= width);

	y += sizer.getHeight() + GAP_Y;
}

CVerticalSizer::~CVerticalSizer() { }
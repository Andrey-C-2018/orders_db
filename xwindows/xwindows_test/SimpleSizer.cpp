#include <algorithm>
#include "SimpleSizer.h"

#undef max

CHorizontalSizer::CHorizontalSizer() noexcept : parent(nullptr) {

	init(MARGIN_X);
}

CHorizontalSizer::CHorizontalSizer(const int margin_x_) noexcept : parent(nullptr) {

	init(margin_x_);
}

CHorizontalSizer::CHorizontalSizer(XWindow *parent_, \
									const int x_, const int y_, \
									const int width_, const int height_, \
									const int margin_x_) noexcept : parent(parent_) {

	init(x_, y_, width_, height_, margin_x_);
}

void CHorizontalSizer::addWidget(XWidget *widget, const int widget_width, const int widget_height) {

	addWidget(widget, _T(""), XSize(widget_width, widget_height));
}

void CHorizontalSizer::addWidget(XWidget *widget, const Tchar *label, XSize size) {
	const int widget_width = size.width;
	const int widget_height = size.height;

	assert(widget);
	assert(x - x_initial + widget_width + margin_x <= width);
	assert(widget_height <= height);

	max_widget_height = std::max(max_widget_height, widget_height);

	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
					label, x, y, widget_width, max_widget_height);
	x += widget_width + GAP_X;
}

void CHorizontalSizer::initNestedSizer(CVerticalSizer &sizer, const int row_height) const {

	sizer = CVerticalSizer(parent, x, y, width - margin_x - x + x_initial, \
							height, row_height, margin_x);
}

void CHorizontalSizer::addNestedSizer(const CVerticalSizer &sizer) {
	
	assert(!sizer.isNull());
	assert(sizer.getHeight() <= height);

	max_widget_height = std::max(max_widget_height, sizer.getHeight());
	x = x_initial + width - margin_x;
}

CHorizontalSizer::~CHorizontalSizer() { }

//****************************************************************************

CVerticalSizer::CVerticalSizer() noexcept : parent(nullptr), x(MARGIN_X), y_initial(0), y(0), \
									margin_x(MARGIN_X), \
									width(0), height(0), row_height(0) { }

CVerticalSizer::CVerticalSizer(const int margin_x_) noexcept : parent(nullptr), x(margin_x_), \
									y_initial(0), y(0), \
									width(0), height(0), 
									margin_x(margin_x_), row_height(0) { }

CVerticalSizer::CVerticalSizer(XWindow *parent_, \
								const int x_, const int y_, \
								const int width_, const int height_, \
								const int row_height_, \
								const int margin_x_) noexcept : \
						parent(parent_), x(x_), y_initial(y_), y(y_ + MARGIN_Y), \
						width(width_), height(height_), \
						margin_x(margin_x_), \
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

	sizer = CHorizontalSizer(parent, x, y, width, \
							height - MARGIN_Y - y + y_initial, margin_x);
}

void CVerticalSizer::addNestedSizer(const CHorizontalSizer &sizer) {

	assert(!sizer.isNull());
	assert(sizer.getWidth() <= width);

	y += sizer.getHeight() + GAP_Y;
}

CVerticalSizer::~CVerticalSizer() { }
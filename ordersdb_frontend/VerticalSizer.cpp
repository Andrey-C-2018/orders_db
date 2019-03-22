#include "VerticalSizer.h"

CVerticalSizer::CVerticalSizer() : parent(nullptr), x(0), y(0), \
					width(0), height(0), row_height(0) { }

CVerticalSizer::CVerticalSizer(XWindow *parent_, \
				const int x_, const int y_, \
				const int width_, const int height_, \
				const int row_height_) : \
			parent(parent_), x(x_), y(y_ + MARGIN_Y), \
			width(width_), height(height_), \
			row_height(row_height_) {
	
	assert(parent);
	assert(width > 0);
	assert(row_height <= height);
}

void CVerticalSizer::add(XWidget *widget) {
	
	assert(widget);
	assert(nested_sizer.isNull());
	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED, _T(""), \
			x, y, width, row_height);

	y += row_height + GAP_Y;
	assert(y <= height);
}

void CVerticalSizer::pushNestedSizer(CHorizontalSizer sizer) {
	
	assert(nested_sizer.isNull());
	nested_sizer = sizer;
}

void CVerticalSizer::popNestedSizer() {
	
	assert(!nested_sizer.isNull());

	y += nested_sizer.getHeight() + GAP_Y;
	nested_sizer.reset();
}

CVerticalSizer::~CVerticalSizer() { }
#include <HorizontalSizer.h>
#include <algorithm>

CHorizontalSizer::CHorizontalSizer(XWindow *parent_, \
				const int x_, const int y_, \
				const int width_, const int height_) : \
			parent(parent_), x(x_ + MARGIN_X), y(y_), \
			width(width_), height(height_), \
			max_widget_height(0) {

	assert(parent);
	assert(height > 0);
}

void CHorizontalSizer::add(XWidget *widget, const int widget_width, const widget_height) {

	max_widget_height = std::max(max_widget_height, widget_height);

	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED, _T(""), \
			x, y, widget_width, max_widget_height);
	x += widget_width + GAP_X;
}

void CHorizontalSizer::pushNestedSizer(CHorizontalSizer sizer) {
	
	assert(nested_sizer.isNull());
	nested_sizer = sizer;
}

void CHorizontalSizer::popNestedSizer() {
	
	assert(!nested_sizer.isNull());

	max_widget_height = std::max(max_widget_height, nested_sizer.getHeight());
	nested_sizer.reset();
}
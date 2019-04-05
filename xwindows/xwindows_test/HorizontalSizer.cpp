#include "HorizontalSizer.h"

CHorizontalSizer::CHorizontalSizer() noexcept : x_initial(0), max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(const int margin_left, const int margin_right, \
									const int margin_top, const int margin_bottom) noexcept : \
					CSizer(margin_left, margin_right, margin_top, margin_bottom), \
					x_initial(0), max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(XWindow *parent, \
									const int x, const int y, \
									const int width, const int height, \
									const int margin_left, const int margin_right, \
									const int margin_top, const int margin_bottom) noexcept : \
					CSizer(parent, x, y, width, height, \
							margin_left, margin_right, margin_top, margin_bottom), \
					x_initial(x), max_widget_height(0) { }

void CHorizontalSizer::onParametersChanged(const int new_x, const int new_y, \
									const int new_width, const int new_height) noexcept {

	x_initial = new_x;
}

void CHorizontalSizer::addWidget(XWidget *widget, const Tchar *label, XSize size) {
	const int widget_width = size.width;
	const int widget_height = size.height;

	assert(widget);
	assert(x - x_initial + widget_width + margin_right <= width);
	assert(widget_height + margin_top + margin_bottom <= height);

	if(widget_height > max_widget_height)
		max_widget_height = widget_height;

	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
					label, x, y, widget_width, widget_height);
	x += widget_width + GAP;
}

void CHorizontalSizer::initNestedSizer(CSizer &sizer) const {

	setParametersTo(sizer, x, y, width + x_initial - x - margin_right, \
									height - margin_top - margin_bottom);
}

void CHorizontalSizer::initNestedSizerWithWidth(CSizer &sizer, const int width) const {

	assert(width <= this->width + x_initial - x - margin_right);
	setParametersTo(sizer, x, y, width, height - margin_top - margin_bottom);
}

void CHorizontalSizer::addNestedSizerByActualSize(const CSizer &sizer) {

	assert(!sizer.isNull());

	int sizer_actual_height = sizer.getActualHeight();
	if (sizer_actual_height > max_widget_height)
		max_widget_height = sizer_actual_height;

	x += sizer.getWidth() + GAP;
}

int CHorizontalSizer::getActualHeight() const {

	return margin_top + max_widget_height + margin_bottom;
}

bool CHorizontalSizer::isNull() const {

	return max_widget_height && isNullBasic();
}

CHorizontalSizer::~CHorizontalSizer() { }
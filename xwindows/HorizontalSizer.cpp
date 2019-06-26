#include "HorizontalSizer.h"

CHorizontalSizer::CHorizontalSizer() noexcept : x_initial(0), max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(const int margin_left, const int margin_right, \
									const int margin_top, const int margin_bottom) noexcept : \
					CSizer(margin_left, margin_right, margin_top, margin_bottom), \
					x_initial(0), max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(CSizerPreferences prefs) noexcept : \
					CSizer(prefs), x_initial(0), max_widget_height(0) { }

CHorizontalSizer::CHorizontalSizer(XWindow *parent, \
									const int x, const int y, \
									const int width, const int height, \
									const int margin_left, const int margin_right, \
									const int margin_top, const int margin_bottom, \
									const int gap) noexcept : \
					CSizer(parent, x, y, width, height, \
							margin_left, margin_right, \
							margin_top, margin_bottom, gap), \
					x_initial(x), max_widget_height(0) { }

void CHorizontalSizer::onParametersChanged(const int new_x, const int new_y, \
									const int new_width, const int new_height) noexcept {

	x_initial = new_x;
}

void CHorizontalSizer::addWidget(XWidget *widget, const Tchar *label, \
									const int flags, XSize size, const int widget_max_height) {

	assert(!nested_sizer);
	const int widget_width = size.width;
	const int widget_height = size.height;

	assert(widget);
	assert(x - x_initial + widget_width + margin_right <= width);
	assert(widget_height + margin_top + margin_bottom <= height);

	if(widget_height > max_widget_height)
		max_widget_height = widget_height;

	widget->Create(parent, flags, label, x, y, widget_width, widget_max_height);
	x += widget_width + gap;
}

void CHorizontalSizer::pushNestedSizer(CSizer &sizer) {

	assert(!nested_sizer);
	setParametersTo(sizer, x, y, width + x_initial - x - margin_right, \
									height - margin_top - margin_bottom);
	nested_sizer = &sizer;
}

void CHorizontalSizer::pushNestedSizer(CSizer &sizer, const int width) {

	assert(!nested_sizer);
	assert(width <= this->width + x_initial - x - margin_right);

	setParametersTo(sizer, x, y, width, height - margin_top - margin_bottom);
	nested_sizer = &sizer;
}

void CHorizontalSizer::popNestedSizer() {

	assert(nested_sizer);
	assert(!nested_sizer->isNull());

	int sizer_actual_height = nested_sizer->getActualHeight();
	if (sizer_actual_height > max_widget_height)
		max_widget_height = sizer_actual_height;

	x += nested_sizer->getWidth() + gap;
	nested_sizer = nullptr;
}

int CHorizontalSizer::getActualHeight() const {

	return margin_top + max_widget_height + margin_bottom;
}

bool CHorizontalSizer::isNull() const {

	return max_widget_height && isNullBasic();
}

CHorizontalSizer::~CHorizontalSizer() { }
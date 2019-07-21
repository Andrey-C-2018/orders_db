#include "VerticalSizer.h"

CVerticalSizer::CVerticalSizer(const int row_height_) noexcept : \
						y_initial(0), row_height(row_height_) { }

CVerticalSizer::CVerticalSizer(const int margin_left, const int margin_right, \
								const int margin_top, const int margin_bottom, \
								const int row_height_) noexcept : \
						CSizer(margin_left, margin_right, margin_top, margin_bottom), \
						y_initial(0), row_height(row_height_) { }

CVerticalSizer::CVerticalSizer(CSizerPreferences prefs, const int row_height_) noexcept : \
						CSizer(prefs), y_initial(0), row_height(row_height_) { }

CVerticalSizer::CVerticalSizer(XWindow *parent, \
								const int x, const int y, \
								const int width, const int height, \
								const int margin_left, const int margin_right, \
								const int margin_top, const int margin_bottom, \
								const int gap, const int row_height_) noexcept : \
						CSizer(parent, x, y, width, height, \
								margin_left, margin_right, \
								margin_top, margin_bottom, gap), \
						y_initial(y), row_height(row_height_) { }

void CVerticalSizer::onParametersChanged(const int new_x, const int new_y, \
										const int new_width, const int new_height) noexcept {

	y_initial = new_y;
}

void CVerticalSizer::addWidget(XWidget *widget, const Tchar *label, const int flags) {

	assert(!nested_sizer);
	assert(widget);
	assert(y - y_initial + row_height + margin_bottom <= height);
	widget->Create(parent, flags, label, x, y, \
					width - margin_left - margin_right, row_height);

	y += row_height + gap;
}

XRect CVerticalSizer::addLastWidget(XWidget *widget) {

	assert(!nested_sizer);
	assert(widget);
	assert(y - y_initial + margin_bottom <= height);
	XRect rc;

	rc.left = x;
	rc.top = y;
	widget->Create(parent, FL_WINDOW_CLIPSIBLINGS | FL_WINDOW_BORDERED | FL_WINDOW_VISIBLE, \
					_T(""), x, y, width - margin_left - margin_right, \
					height + y_initial - y - margin_bottom);

	y = height - margin_bottom;
	rc.right = width - margin_left - margin_right;
	rc.bottom = y;
	return rc;
}

void CVerticalSizer::pushNestedSizer(CSizer &sizer) {

	assert(!nested_sizer);
	setParametersTo(sizer, x, y, width - margin_left - margin_right, \
					height + y_initial - y - margin_bottom);
	nested_sizer = &sizer;
}

void CVerticalSizer::popNestedSizer() {

	assert(nested_sizer);
	assert(!nested_sizer->isNull());

	int actual_height = nested_sizer->getActualHeight();
	y += actual_height + gap;
	nested_sizer = nullptr;
}

int CVerticalSizer::getActualHeight() const {
	
	return y - y_initial - gap + margin_bottom;
}

bool CVerticalSizer::isNull() const {

	return y_initial == y - margin_top || isNullBasic();
}

CVerticalSizer::~CVerticalSizer() { }
#include "Sizer.h"

CSizer::CSizer() noexcept : parent(nullptr) {

	init(MARGIN_X, MARGIN_X, MARGIN_Y, MARGIN_Y);
}

CSizer::CSizer(const int margin_left, const int margin_right, \
				const int margin_top, const int margin_bottom) noexcept : \
			parent(nullptr) {

	init(margin_left, margin_right, margin_top, margin_bottom);
}

CSizer::CSizer(XWindow *parent_, \
				const int x, const int y, \
				const int width, const int height, \
				const int margin_left, const int margin_right, \
				const int margin_top, const int margin_bottom) noexcept : \
			parent(parent_) {

	assert(width > 0);
	assert(height > 0);
	init(x, y, width, height, margin_left, margin_right, \
								margin_top, margin_bottom);
}

bool CSizer::isNull() const {

	return isNullBasic();
}

CSizer::~CSizer() { }
#include "Sizer.h"

CSizer::CSizer() noexcept : parent(nullptr) {

	init(0, 0, 2 * MARGIN_X, 2 * MARGIN_Y, MARGIN_X, MARGIN_X, MARGIN_Y, MARGIN_Y, GAP);
}

CSizer::CSizer(const int margin_left, const int margin_right, \
				const int margin_top, const int margin_bottom) noexcept : \
			parent(nullptr) {

	init(0, 0, margin_left + margin_right, margin_top + margin_bottom, \
				margin_left, margin_right, margin_top, margin_bottom, GAP);
}

CSizer::CSizer(CSizerPreferences prefs) noexcept {

	init(0, 0, prefs.margin_left + prefs.margin_right, \
				prefs.margin_top + prefs.margin_bottom, \
				prefs.margin_left, prefs.margin_right, \
				prefs.margin_top, prefs.margin_bottom, prefs.gap);
}

CSizer::CSizer(XWindow *parent_, \
				const int x, const int y, \
				const int width, const int height, \
				const int margin_left, const int margin_right, \
				const int margin_top, const int margin_bottom, \
				const int gap) noexcept : \
			parent(parent_) {

	assert(width > 0);
	assert(height > 0);
	init(x, y, width, height, margin_left, margin_right, \
								margin_top, margin_bottom, gap);
}

bool CSizer::isNull() const {

	return isNullBasic();
}

CSizer::~CSizer() { }
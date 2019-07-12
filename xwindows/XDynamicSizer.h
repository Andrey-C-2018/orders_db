#pragma once
#include "platform_specific.h"

class XDynamicSizer final {
	XPoint initial_coords;
	XSize parent_size;

	int margin_right, margin_bottom;
	float multiplier;
	int minus;

public:
	XDynamicSizer();
	XDynamicSizer(const int margin_right, const int margin_bottom);
	XDynamicSizer(XPoint initial_coords_, XSize parent_size_, \
					const int margin_right, const int margin_bottom);

	XDynamicSizer(const XDynamicSizer &obj) = default;
	XDynamicSizer &operator=(const XDynamicSizer &obj) = default;

	inline int getBottom() const;

	inline void setCoords(XPoint initial_coords);
	inline void setHeightMultiplier(const float multiplier);
	inline void setMinus(const int minus);

	inline XRect resize() const;
	inline XRect resize(XSize parent_size);
	inline XRect resize(XDynamicSizer *prev_sizer);

	~XDynamicSizer();
};

//*****************************************************

void XDynamicSizer::setCoords(XPoint initial_coords) {

	this->initial_coords = initial_coords;
}

void XDynamicSizer::setHeightMultiplier(const float multiplier) {

	assert(1.0F - multiplier >= 1e-6);
	this->multiplier = multiplier;
}

void XDynamicSizer::setMinus(const int minus) {

	this->minus = minus;
}

int XDynamicSizer::getBottom() const {

	return (int)((float)parent_size.height * multiplier) - \
				margin_bottom - minus;
}

XRect XDynamicSizer::resize() const {

	XRect r;
	r.left = initial_coords.x;
	r.top = initial_coords.y;
	r.right = parent_size.width - margin_right;
	r.bottom = getBottom();

	return r;
}

XRect XDynamicSizer::resize(XSize parent_size) {

	this->parent_size = parent_size;
	return resize();
}

XRect XDynamicSizer::resize(XDynamicSizer *prev_sizer) {

	assert(prev_sizer);
	setCoords(XPoint(prev_sizer->initial_coords.x, prev_sizer->getBottom()));
	
	return resize(prev_sizer->parent_size);
}
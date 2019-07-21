#pragma once
#include <xwindows/XWidget.h>

class XDynamicSizer final {
	XPoint initial_coords;
	XSize parent_size, size;
	int width_key, height_key;
		
	int margin_right, margin_bottom;
	int minus_x, minus_y;
	float multiplier_x, multiplier_y;

public:
	XDynamicSizer();
	XDynamicSizer(const int margin_right, const int margin_bottom);
	XDynamicSizer(XPoint initial_coords_, XSize parent_size_, \
					const int margin_right, const int margin_bottom);

	XDynamicSizer(const XDynamicSizer &obj) = default;
	XDynamicSizer(XDynamicSizer &&obj) = default;
	XDynamicSizer &operator=(const XDynamicSizer &obj) = default;
	XDynamicSizer &operator=(XDynamicSizer &&obj) = default;

	inline int getRight() const;
	inline int getBottom() const;

	inline void setCoords(XPoint initial_coords);
	inline bool setParentSize(XSize parent_size);
	inline void freezeWidth();
	inline void freezeHeight();

	inline void setHeightMultiplier(const float multiplier);
	inline void setMinusX(const int minus_x);
	inline void setMinusY(const int minus_y);
	inline void setRelativeWidth(const int width);
	inline void setRelativeHeight(const int height);
	inline void setRelativeSize(XSize size);

	inline void addToVertSizersChain(const XDynamicSizer &prev_sizer);
	inline void addToHorzSizersChain(const XDynamicSizer &prev_sizer);
	inline void createWidget(XWidget *widget, XWindow *parent, const int flags, \
							const Tchar *label) const;

	inline void updateVertSizersChain(const XDynamicSizer &prev_sizer);
	inline void updateHorzSizersChain(const XDynamicSizer &prev_sizer);
	inline void moveWidget(XWidget *widget) const;
	
	~XDynamicSizer();
};

//*****************************************************

void XDynamicSizer::setCoords(XPoint initial_coords) {

	this->initial_coords = initial_coords;
}

void XDynamicSizer::freezeWidth() {

	assert(1.0F - multiplier_x <= 1e-6);
	width_key = 0;
	size.width = getRight() - initial_coords.x;
}

void XDynamicSizer::freezeHeight() {

	assert(1.0F - multiplier_y <= 1e-6);
	height_key = 0;
	size.height = getBottom() - initial_coords.y;
}

bool XDynamicSizer::setParentSize(XSize parent_size) {

	bool no_op = parent_size.width != 0 && parent_size.height != 0 && \
					parent_size != this->parent_size;
	this->parent_size = parent_size;
	return no_op;
}

void XDynamicSizer::setHeightMultiplier(const float multiplier) {

	assert(1.0F - multiplier >= 1e-6);
	this->multiplier_y = multiplier;
}

void XDynamicSizer::setMinusX(const int minus_x) {

	this->minus_x = minus_x;
}

void XDynamicSizer::setMinusY(const int minus_y) {

	this->minus_y = minus_y;
}

void XDynamicSizer::setRelativeWidth(const int width) {

	this->minus_x = parent_size.width - width - initial_coords.x;
}

void XDynamicSizer::setRelativeHeight(const int height) {

	this->minus_y = parent_size.height - height - initial_coords.y;
}

void XDynamicSizer::setRelativeSize(XSize size) {

	this->minus_x = parent_size.width - size.width;
	this->minus_y = parent_size.height - size.height;
}

int XDynamicSizer::getRight() const {

	return (int)(multiplier_x * (float)(parent_size.width - minus_x)) - margin_right;
}

int XDynamicSizer::getBottom() const {

	return (int)(multiplier_y * (float)(parent_size.height - minus_y)) - margin_bottom;
}

void XDynamicSizer::addToVertSizersChain(const XDynamicSizer &prev_sizer) {

	assert(this != &prev_sizer);
	initial_coords.x = prev_sizer.initial_coords.x;
	initial_coords.y = prev_sizer.getBottom() + prev_sizer.margin_bottom;
	setParentSize(prev_sizer.parent_size);
	minus_x = prev_sizer.minus_x;

	if (!prev_sizer.width_key) freezeWidth();
}

void XDynamicSizer::addToHorzSizersChain(const XDynamicSizer &prev_sizer) {

	assert(this != &prev_sizer);
	initial_coords.x = prev_sizer.getRight() + prev_sizer.margin_right;
	initial_coords.y = prev_sizer.initial_coords.y;
	setParentSize(prev_sizer.parent_size);
	//minus_y = prev_sizer.minus_y;
}

void XDynamicSizer::createWidget(XWidget *widget, XWindow *parent, const int flags, \
									const Tchar *label) const {

	assert(widget && !widget->IsCreated());

	int width = width_key * (getRight() - initial_coords.x) + !width_key * size.width;
	int height = height_key * (getBottom() - initial_coords.y) + !height_key * size.height;
	
	widget->Create(parent, flags, label, initial_coords.x, initial_coords.y, \
					width, height);
}

void XDynamicSizer::updateVertSizersChain(const XDynamicSizer &prev_sizer) {

	assert(this != &prev_sizer);

	initial_coords.y = prev_sizer.height_key * (prev_sizer.getBottom() + \
												prev_sizer.margin_bottom) + \
						!prev_sizer.height_key * (prev_sizer.initial_coords.y + \
												prev_sizer.size.height + \
												prev_sizer.margin_bottom);

	setParentSize(prev_sizer.parent_size);
}

void XDynamicSizer::updateHorzSizersChain(const XDynamicSizer &prev_sizer) {

	assert(this != &prev_sizer);

	initial_coords.x = prev_sizer.width_key * (prev_sizer.getRight() + \
												prev_sizer.margin_right) + \
						!prev_sizer.width_key * (prev_sizer.initial_coords.x + \
												prev_sizer.size.width + \
												prev_sizer.margin_right);

	setParentSize(prev_sizer.parent_size);
}

void XDynamicSizer::moveWidget(XWidget *widget) const {

	assert(widget);
	int width = width_key * (getRight() - initial_coords.x) + !width_key * size.width;
	int height = height_key * (getBottom() - initial_coords.y) + !height_key * size.height;

	widget->MoveWindow(initial_coords.x, initial_coords.y, \
						width, height);
}

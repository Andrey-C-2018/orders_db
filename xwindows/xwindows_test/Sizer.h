#pragma once
#include <assert.h>

class XWindow;

class CSizer {
protected:
	enum Defaults {
		MARGIN_X = 5, \
		MARGIN_Y = 5, \
		GAP = 5
	};

	mutable XWindow *parent;
	int flags;

	int x, y;
	int width, height;
	int margin_left, margin_right;
	int margin_top, margin_bottom;
	int gap;

	inline void init(const int margin_left, const int margin_right, \
						const int margin_top, const int margin_bottom) noexcept;
	inline void init(const int x, const int y, \
						const int width, const int height, \
						const int margin_left, const int margin_right, \
						const int margin_top, const int margin_bottom) noexcept;

	inline void setParametersTo(CSizer &obj, const int new_x, const int new_y, \
									const int new_width, const int new_height) const noexcept;
	virtual void onParametersChanged(const int new_x, const int new_y, \
									const int new_width, const int new_height) noexcept { }
	inline bool isNullBasic() const;
public:
	CSizer() noexcept;
	CSizer(const int margin_left, const int margin_right, \
			const int margin_top, const int margin_bottom) noexcept;
	CSizer(XWindow *parent, \
			const int x, const int y, \
			const int width, const int height, \
			const int margin_left, const int margin_right, \
			const int margin_top, const int margin_bottom) noexcept;

	CSizer(const CSizer &obj) = default;
	CSizer(CSizer &&obj) = default;
	CSizer &operator=(const CSizer &obj) = default;
	CSizer &operator=(CSizer &&obj) = default;

	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }
	virtual int getActualHeight() const = 0;

	inline void setWidgetCreationFlags(const int flags);
	inline void setGap(const int gap);

	virtual bool isNull() const;
	virtual ~CSizer();
};

//****************************************************************************

void CSizer::init(const int margin_left, const int margin_right, \
					const int margin_top, const int margin_bottom) noexcept {

	init(0, 0, margin_left + margin_right, margin_top + margin_bottom, \
			margin_left, margin_right, margin_top, margin_bottom);
}

void CSizer::init(const int x, const int y, \
					const int width, const int height, \
					const int margin_left, const int margin_right, \
					const int margin_top, const int margin_bottom) noexcept {

	assert(margin_left + margin_right <= width);
	assert(margin_top + margin_bottom <= height);

	this->x = x + margin_left;
	this->y = y + margin_top;

	this->width = width;
	this->height = height;

	this->margin_left = margin_left;
	this->margin_right = margin_right;
	this->margin_top = margin_top;
	this->margin_bottom = margin_bottom;

	this->gap = GAP;
	this->flags = 0;
}

void CSizer::setParametersTo(CSizer &obj, const int new_x, const int new_y, \
									const int new_width, const int new_height) const noexcept {

	obj.parent = this->parent;
	obj.flags = this->flags;
	obj.gap = this->gap;

	obj.x = new_x + obj.margin_left;
	obj.y = new_y + obj.margin_top;
	obj.width = new_width;
	obj.height = new_height;

	obj.onParametersChanged(new_x, new_y, new_width, new_height);
}

bool CSizer::isNullBasic() const {

	return !(parent && width > margin_left + margin_right && \
				height > margin_top + margin_bottom);
}
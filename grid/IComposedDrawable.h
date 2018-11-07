#pragma once
#include "IDrawable.h"

struct IComposedDrawable : public IDrawable {
	virtual void SetItemHeight(const int height) = 0;
	virtual IDrawable &GetByIndex(const size_t index) = 0;
	virtual size_t count() const = 0;
	virtual ~IComposedDrawable() { }
};

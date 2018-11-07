#pragma once

class XGC;
struct XPoint;
struct XSize;

struct IDrawable {
	virtual void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) = 0;
	virtual void SetBounds(const int left_bound, const int upper_bound) = 0;
	virtual ~IDrawable() { }
};

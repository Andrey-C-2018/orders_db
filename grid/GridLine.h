#pragma once
#include "IDrawable.h"
#include <xwindows/XGC.h>

class CGridLine{
	int grid_line_size;
	XBrush *bkgrnd_brush;
	XPen *bkgrnd_pen;

protected:
	enum {
		DEF_GRID_LINE_SIZE = 2
	};
	
public:
	CGridLine() : grid_line_size(DEF_GRID_LINE_SIZE), \
				bkgrnd_brush(nullptr){ }

	CGridLine(const CGridLine &obj) = default;
	CGridLine(CGridLine &&obj) = default;

	inline int GetGridLineSize() const { return grid_line_size; }

	inline void SetBackground(XBrush *bkgrnd, XPen *pen) {

		this->bkgrnd_brush = bkgrnd;
		this->bkgrnd_pen = pen;
	}

	inline void FillEmptySpace(XGC &gc, const int x1, const int y1, \
										const int x2, const int y2){
		XBrush old_brush = gc.SelectObject(*bkgrnd_brush);
		XPen old_pen = gc.SelectObject(*bkgrnd_pen);

		gc.RectangleWithBounds(x1, y1, x2, y2);

		gc.SelectObject(old_pen);
		gc.SelectObject(old_brush);
	}
};

class CHorizontalLine : public CGridLine {
public:
	CHorizontalLine() : CGridLine() { }

	CHorizontalLine(const CHorizontalLine &obj) = default;
	CHorizontalLine(CHorizontalLine &&obj) = default;

	inline XSize Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) {

		gc.RectangleWithBounds(initial_coords.x, \
						initial_coords.y + size.height - GetGridLineSize(), \
						initial_coords.x + size.width, \
						initial_coords.y + size.height);

		return GetNewSize(size);
	}

	inline XSize GetNewSize(const XSize &size) const {

		return XSize(size.width, size.height - GetGridLineSize());
	}
};

class CVerticalLine : public CGridLine {
public:
	CVerticalLine() : CGridLine() { }

	CVerticalLine(const CVerticalLine &obj) = default;
	CVerticalLine(CVerticalLine &&obj) = default;

	inline XSize Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) {
		
		gc.RectangleWithBounds(initial_coords.x + size.width - GetGridLineSize(), \
						initial_coords.y, \
						initial_coords.x + size.width, \
						initial_coords.y + size.height);

		return GetNewSize(size);
	}

	inline XSize GetNewSize(const XSize &size) const {

		return XSize(size.width - GetGridLineSize(), size.height);
	}
};

template <class Cell, class Line> \
class TCellWithLine : public Cell{
	Line line;
public:
	inline TCellWithLine() : line() { }

	inline TCellWithLine(const int grid_line_size_) : \
							line(grid_line_size_){ }

	template <class Data> \
	inline TCellWithLine(Data data) : Cell(data){ }

	TCellWithLine(const TCellWithLine &obj) = default;
	TCellWithLine(TCellWithLine &&obj) = default;

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {
		XSize new_size = line.Draw(gc, initial_coords, size);

		Cell::Draw(gc, initial_coords, new_size);
	}
};

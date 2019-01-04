#pragma once
#include <memory>
#include <xwindows/XGC.h>
#include "IConfigurator.h"

class CGridTableProxy;

class CCellConfigurator : public IConfigurator {
	int text_height;
	int cell_height, cell_margins_width;
	int char_width;
	int grid_line_size;

	XBrush *background_brush;
	XPen *background_pen;
	
	inline void InternalInit(XGC *gc, XFont *font, \
							XBrush *background_brush_, \
							XPen *background_pen_) {
		assert(gc && font);
		assert(background_brush_);
		assert(background_pen_);

		this->background_brush = background_brush_;
		this->background_pen = background_pen_;

		XFont old_font = gc->SelectObject(*font);
		XTextMetric tm(gc);

		text_height = tm.GetCharHeight();
		char_width = tm.GetAverageCharWidth();
		gc->SelectObject(old_font);
	}

public:
	CCellConfigurator() : text_height(-1), cell_height(-1), \
							cell_margins_width(-1), \
							char_width(-1), \
							grid_line_size(-1), \
							background_brush(nullptr), \
							background_pen(nullptr){ }

	CCellConfigurator(const CCellConfigurator &obj) = delete;
	CCellConfigurator(CCellConfigurator &&obj) = default;
	CCellConfigurator &operator=(const CCellConfigurator &obj) = delete;
	CCellConfigurator &operator=(CCellConfigurator &&obj) = default;

	CCellConfigurator(XGC *gc, XFont *font, \
						XBrush *background_brush_, \
						XPen *background_pen_) : cell_height(-1) {

		InternalInit(gc, font, background_brush_, background_pen_);
	}

	void Init(XGC *gc, XFont *font, \
						XBrush *background_brush_, \
						XPen *background_pen_) {

		InternalInit(gc, font, background_brush_, background_pen_);
	}

	void Configure(CGridLine &line) override;

	void Configure(CTextCell &text_cell) override;

	void Configure(CDispatcherCell &disp_cell) override;

	inline int GetCellHeight() const {

		assert(cell_height != -1);
		assert(grid_line_size != -1);
		return cell_height + grid_line_size;
	}

	inline int GetCellMarginsWidth() const {

		assert(cell_margins_width != -1);
		return cell_margins_width + grid_line_size;
	}

	inline int GetCharWidthInPixels() const {

		assert(char_width != -1);
		return char_width;
	}

	virtual ~CCellConfigurator() { }
};
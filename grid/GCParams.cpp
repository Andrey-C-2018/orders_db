#include "GCParams.h"

CGCParams::CGCParams(XFont *font_, XColor background_color) : \
					old_bk_mode(0), bg_color(background_color), \
					font(font_) { }

void CGCParams::SetParametersToGC(XGC *gc){

	assert(gc);
	assert(old_font.IsNull());
	assert(old_bg_color.IsNull());
	old_bk_mode = gc->SetBackgroundFillMode(X_BK_TRANSPARENT);
	old_bg_color = gc->SetBackgroundColor(bg_color);
	old_font = gc->SelectObject(*font);
}

void CGCParams::RestoreDefaultGCParameters(XGC *gc){

	assert(gc);
	assert(!old_font.IsNull());
	assert(!old_bg_color.IsNull());
	gc->SelectObject(old_font);
	gc->SetBackgroundColor(old_bg_color);
	gc->SetBackgroundFillMode(old_bk_mode);

	old_font.Reset();
	old_bg_color.Reset();
}
#pragma once
#include <basic/ImmutableString.h>
#include <xwindows/XGC.h>
#include "IDrawable.h"

class CTextCell : public IDrawable {
	const Tchar *cell_text;
	size_t cell_text_size;
	bool is_null;
	int margin_x, margin_y;

	XColor null_values_color;

	enum Defaults {
		DEF_MARGIN_X = 2, \
		DEF_MARGIN_Y = 2
	};

public:
	CTextCell() : cell_text(nullptr), \
					cell_text_size(0), \
					is_null(true), \
					margin_x(DEF_MARGIN_X), \
					margin_y(DEF_MARGIN_Y) , \
					null_values_color(200, 200, 255) { }

	CTextCell(const CTextCell &obj) = default;
	CTextCell &operator=(const CTextCell &obj) = default;
	CTextCell(CTextCell &&obj) = default;
	CTextCell &operator=(CTextCell &&obj) = default;

	inline void Init(const ImmutableString<Tchar> str, \
						const size_t field, const size_t record) {

		if (!str.str) {
			this->cell_text = _T("NULL");
			this->cell_text_size = 4;
			is_null = true;
		}
		else {
			this->cell_text = str.str;
			this->cell_text_size = str.size;
			is_null = false;
		}
	}

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override {
		XRect rc;

		rc.left = initial_coords.x;
		rc.top = initial_coords.y;
		rc.right = initial_coords.x + size.width;
		rc.bottom = initial_coords.y + size.height;

		XColor old_color;
		if(is_null)	old_color = gc.SetBackgroundColor(null_values_color);

		gc.DisplayTextWithBounds(initial_coords.x + margin_x, \
						initial_coords.y + margin_y, \
						XTEXTOUT_CLIPPED | XTEXTOUT_OPAQUE, rc, \
						cell_text, cell_text_size);

		if (is_null) gc.SetBackgroundColor(old_color);
	}

	inline void BeginScroll(const int scroll_pos) {	}

	inline void EndScroll(const int scroll_pos) { }

	void SetBounds(const int left_bound, const int upper_bound) override { }

	inline void OnClick(const size_t field, const size_t record) {	}

	inline void OnFieldRemoved(const size_t field) { }

	inline int EvalCellHeightByTextHeight(const int text_height) const{

		return text_height + 2 * margin_y;
	}

	inline int GetMarginsWidth() const {

		return 2 * margin_x;
	}

	inline void Reload() { }

	virtual ~CTextCell() { }
};

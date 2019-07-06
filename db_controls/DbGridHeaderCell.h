#pragma once
#include <grid/TextCell.h>

class CDbGridHeaderCell :	public CTextCell {
	int arrow_state;
	CDelegate on_click_action;

	inline void DrawArrow();
public:
	CDbGridHeaderCell();

	CDbGridHeaderCell(const CDbGridHeaderCell &obj) = default;
	CDbGridHeaderCell &operator=(const CDbGridHeaderCell &obj) = default;
	CDbGridHeaderCell(CDbGridHeaderCell &&obj) = default;
	CDbGridHeaderCell &operator=(CDbGridHeaderCell &&obj) = default;

	void Draw(XGC &gc, const XPoint &initial_coords, const XSize &size) override;
	inline void OnClick(const size_t field, const size_t record);

	virtual ~CDbGridHeaderCell();
};


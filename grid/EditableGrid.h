#pragma once
#include "Grid.h"

class CEditableGrid : public CGrid{
	CEditableTextCell *editable_cell;

	void OnKeyPress(XKeyboardEvent *eve);
protected:
	LayoutObjects CreateLayoutObjects(const int kind_of_layout) override;
	void OnWindowCreate() override;

	CEditableGrid(const CEditableGrid &obj) = delete;
	CEditableGrid &operator=(const CEditableGrid &obj) = delete;
public:
	CEditableGrid();

	CEditableGrid(CEditableGrid &&obj) = default;
	CEditableGrid &operator=(CEditableGrid &&obj) = default;

	virtual ~CEditableGrid();
};


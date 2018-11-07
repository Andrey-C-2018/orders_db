#pragma once
#include "Grid.h"

class CEditableGrid : public CGrid{
	CEditableTextCell *editable_cell;

	void OnKeyPress(XKeyboardEvent *eve);
protected:
	LayoutObjects CreateLayoutObjects(const int kind_of_layout) final;
	void OnWindowCreate() override;
public:
	CEditableGrid();
	virtual ~CEditableGrid();
};


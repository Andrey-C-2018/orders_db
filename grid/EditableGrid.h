#pragma once
#include "Grid.h"

struct IGridCellWidget;
struct IGridEventsHandler;
class CMultipleCellWidget;

class CEditableGrid : public CGrid{
	CDispatcherCell *disp_cell;
	CMultipleCellWidget *field_widgets_collection;
	std::shared_ptr<IGridEventsHandler> events_handler;

	void OnKeyPress(XKeyboardEvent *eve);

protected:
	LayoutObjects CreateLayoutObjects(const int kind_of_layout) override;
	void OnWindowCreate() override;

	CEditableGrid(const CEditableGrid &obj) = delete;
	CEditableGrid &operator=(const CEditableGrid &obj) = delete;
public:
	CEditableGrid(std::unique_ptr<IGridEventsHandler> events_handler_);

	CEditableGrid(CEditableGrid &&obj);
	CEditableGrid &operator=(CEditableGrid &&obj);

	void SetWidgetForField(const size_t field, IGridCellWidget *field_widget);

	virtual ~CEditableGrid();
};


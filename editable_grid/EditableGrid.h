#pragma once
#include <grid/Grid.h>

struct IGridCellWidget;
struct IGridEventsHandler;
class CMultipleCellWidget;
class CEConfigurator;

class CEditableGrid : public CGrid{

	CDispatcherCell *disp_cell;
	CMultipleCellWidget *field_widgets_collection;
	std::shared_ptr<IGridEventsHandler> events_handler;
	
	XFont cells_font, headers_font;
	XColor cells_color, headers_color;
	XBrush grid_lines_brush, background_brush;
	XPen grid_lines_pen, background_pen;

	void OnKeyPress(XKeyboardEvent *eve);

protected:
	GridConfigurators CreateConfigurators() override;
	GridGCParamsLists CreateGCParamsLists() override;

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


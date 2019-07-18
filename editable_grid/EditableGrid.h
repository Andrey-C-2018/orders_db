#pragma once
#include <grid/Grid.h>
#include "ITabKeyAction.h"

struct IGridCellWidget;
struct IGridEventsHandler;
class CMultipleCellWidget;
class CEConfigurator;

class CEditableGrid : public CGrid{
	bool has_focus;
	CMultipleCellWidget *field_widgets_collection;
	std::shared_ptr<IGridEventsHandler> events_handler;
	std::shared_ptr<CEConfigurator> ce_configurator;
	
	XFont cells_font, headers_font;
	XColor cells_color, headers_color;
	XBrush grid_lines_brush, background_brush;
	XPen grid_lines_pen, background_pen;

	void OnMouseMove(XMouseEvent *eve);
	void OnLooseFocus(XMouseEvent *eve);
	void InitEditableGrid();
protected:
	GridConfigurators CreateConfigurators() override;
	GridGCParamsLists CreateGCParamsLists() override;

	LayoutObjects CreateLayoutObjects(const int kind_of_layout) override;
	void OnWindowCreate() override;

	inline void SetEventHandler(std::shared_ptr<IGridEventsHandler> events_handler) {

		assert(!ce_configurator);
		this->events_handler = events_handler;
	}
public:
	CEditableGrid();
	CEditableGrid(std::shared_ptr<IGridEventsHandler> events_handler_);

	CEditableGrid(CEditableGrid &&obj);
	CEditableGrid(const CEditableGrid &obj) = delete;
	CEditableGrid &operator=(CEditableGrid &&obj);
	CEditableGrid &operator=(const CEditableGrid &obj) = delete;

	void SetWidgetForField(const size_t field, IGridCellWidget *field_widget);
	void OnKeyPress(XKeyboardEvent *eve);

	virtual ~CEditableGrid();
};


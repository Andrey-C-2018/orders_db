#include <grid/CellConfigurator.h>
#include "EditableGrid.h"
#include "DispatcherCell.h"
#include "MultipleCellWidget.h"
#include "EditableCellWidget.h"

struct CellDrawingParams {
	XFont *cell_font;
	XPen *cell_pen;
	XBrush *cell_brush;
};

struct DispatcherCellParams {
	XWindow *parent;
	std::shared_ptr<IGridEventsHandler> events_handler;
	std::shared_ptr<CGridTableProxy> table_proxy;
	CMultipleCellWidget *field_widgets_collection;
};

class CEConfigurator final : public CCellConfigurator {
	XWindow *parent;
	std::shared_ptr<IGridEventsHandler> events_handler;
	std::shared_ptr<CGridTableProxy> table_proxy;

	CDispatcherCell *disp_cell;
	CMultipleCellWidget *field_widgets_collection;
public:
	CEConfigurator(XGC *gc, CellDrawingParams drawing_params, \
					DispatcherCellParams disp_cell_params) :
						CCellConfigurator(gc, drawing_params.cell_font, \
											drawing_params.cell_brush, \
											drawing_params.cell_pen), \
						parent(disp_cell_params.parent), \
						events_handler(disp_cell_params.events_handler), \
						table_proxy(disp_cell_params.table_proxy), \
						disp_cell(nullptr), \
						field_widgets_collection(disp_cell_params.field_widgets_collection) { }

	void Configure(CDispatcherCell &disp_cell_) override {

		this->disp_cell = &disp_cell_;
		IGridCellWidget *cell_widget = field_widgets_collection;
		this->disp_cell->SetParameters(parent, events_handler, cell_widget, table_proxy);

		SetCellHeightAndMargins(disp_cell->EvalCellHeightByTextHeight(GetTextHeight()), \
								disp_cell->GetMarginsWidth());
	}

	inline CDispatcherCell *GetDispatcherCell() const {

		return disp_cell;
	}

	virtual ~CEConfigurator() { }
};

//**************************************************************

class CGCParamsList final : public IGCParamsList {
	XGC *gc;
	XFont *cells_font, old_font;
	XColor *cells_color, old_bg_color;
	XBrush *grid_lines_brush, old_brush;
	XPen *grid_lines_pen, old_pen;
	int old_bk_mode;
public:

	CGCParamsList(XFont &cells_font_, XColor &cells_color_, \
					XBrush &grid_lines_brush_, XPen &grid_lines_pen_) : gc(nullptr), \
				cells_font(&cells_font_), cells_color(&cells_color_), \
				grid_lines_brush(&grid_lines_brush_), grid_lines_pen(&grid_lines_pen_) { }

	void Init(XGC &gc) override {

		assert(!this->gc);
		this->gc = &gc;
		old_bk_mode = gc.SetBackgroundFillMode(X_BK_TRANSPARENT);
		old_bg_color = gc.SetBackgroundColor(*cells_color);
		old_font = gc.SelectObject(*cells_font);
		old_pen = gc.SelectObject(*grid_lines_pen);
		old_brush = gc.SelectObject(*grid_lines_brush);
	}

	void Release() override {

		assert(gc);
		gc->SelectObject(old_pen);
		gc->SelectObject(old_brush);
		gc->SelectObject(old_font);
		gc->SetBackgroundColor(old_bg_color);
		gc->SetBackgroundFillMode(old_bk_mode);

		gc = nullptr;
	}
};

//**************************************************************

class CHeadersGCParamsList final : public IGCParamsList {
	XColor *headers_color;
	XFont *headers_font;
public:

	CHeadersGCParamsList(XColor &headers_color_, \
							XFont &headers_font_) : \
						headers_color(&headers_color_), \
						headers_font(&headers_font_) { }

	void Init(XGC &gc) override {

		gc.SetBackgroundColor(*headers_color);
		gc.SelectObject(*headers_font);
	}

	void Release() override { }
};

//**************************************************************

CEditableGrid::CEditableGrid(std::unique_ptr<IGridEventsHandler> events_handler_) : \
									CGrid(), field_widgets_collection(nullptr), \
									events_handler(std::move(events_handler_)), \
									cells_font(20, 0, 0, 0, RUSSIAN_CHARSET, _T("Consolas")), \
									headers_font(20, 0, 0, 0, RUSSIAN_CHARSET, _T("Arial")), \
									cells_color(170, 170, 170), headers_color(128, 128, 128), \
									grid_lines_brush(0, 0, 200), background_brush(100, 100, 100), \
									grid_lines_pen(XPEN_SOLID, 1, 0, 0, 200), \
									background_pen(XPEN_SOLID, 1, 100, 100, 100) {

	assert(events_handler);
	field_widgets_collection = new CMultipleCellWidget();
	field_widgets_collection->SetDefaultWidget(new CEditableCellWidget());
}

GridConfigurators CEditableGrid::CreateConfigurators() {

	XStandAloneGC gc(this);
	CellDrawingParams cell_drawing_params;
	cell_drawing_params.cell_font = &headers_font;
	cell_drawing_params.cell_brush = &background_brush;
	cell_drawing_params.cell_pen = &background_pen;

	auto headers_configurator = std::make_shared<CCellConfigurator>(&gc, &headers_font, \
														&background_brush, &background_pen);

	cell_drawing_params.cell_font = &cells_font;

	DispatcherCellParams disp_cell_params;
	disp_cell_params.parent = this;
	disp_cell_params.table_proxy = data_table_proxy;
	disp_cell_params.field_widgets_collection = field_widgets_collection;
	disp_cell_params.events_handler = events_handler;

	auto cells_configurator = std::make_shared<CEConfigurator>(&gc, cell_drawing_params, \
														disp_cell_params);

	ce_configurator = cells_configurator;

	return GridConfigurators(std::move(headers_configurator), \
								std::move(cells_configurator));
}

GridGCParamsLists CEditableGrid::CreateGCParamsLists() {

	auto global_params = std::make_shared<CGCParamsList>(cells_font, cells_color, \
														grid_lines_brush, grid_lines_pen);
	auto headers_params = std::make_shared<CHeadersGCParamsList>(headers_color, headers_font);

	return GridGCParamsLists(global_params, headers_params);
}

CEditableGrid::CEditableGrid(CEditableGrid &&obj) : CGrid(std::move(obj)){

	field_widgets_collection = obj.field_widgets_collection;
	events_handler = std::move(obj.events_handler);
	ce_configurator = std::move(obj.ce_configurator);

	cells_font = std::move(obj.cells_font);
	headers_font = std::move(obj.headers_font);
	cells_color = std::move(obj.cells_color);
	headers_color = std::move(obj.headers_color);
	grid_lines_brush = std::move(obj.grid_lines_brush);
	background_brush = std::move(obj.background_brush);
	grid_lines_pen = std::move(obj.grid_lines_pen);
	background_pen = std::move(obj.background_pen);

	obj.field_widgets_collection = nullptr;
}

CEditableGrid &CEditableGrid::operator=(CEditableGrid &&obj) {

	CGrid::operator=(std::move(obj));

	field_widgets_collection = obj.field_widgets_collection;
	events_handler = std::move(obj.events_handler);
	ce_configurator = std::move(obj.ce_configurator);

	cells_font = std::move(obj.cells_font);
	headers_font = std::move(obj.headers_font);
	cells_color = std::move(obj.cells_color);
	headers_color = std::move(obj.headers_color);
	grid_lines_brush = std::move(obj.grid_lines_brush);
	background_brush = std::move(obj.background_brush);
	grid_lines_pen = std::move(obj.grid_lines_pen);
	background_pen = std::move(obj.background_pen);

	obj.field_widgets_collection = nullptr;

	return *this;
}

LayoutObjects CEditableGrid::CreateLayoutObjects(const int kind_of_layout) {

	return CreateLayoutObjectsHelper<CDispatcherCell>(kind_of_layout);
}

void CEditableGrid::OnWindowCreate() {

	Connect(EVT_KEYDOWN, this, &CEditableGrid::OnKeyPress);
}

void CEditableGrid::SetWidgetForField(const size_t field, IGridCellWidget *field_widget) {

	assert(field_widget);
	assert(!IsCreated());
	field_widgets_collection->AddCellWidget(field, field_widget);
}

void CEditableGrid::OnKeyPress(XKeyboardEvent *eve) {

	auto disp_cell = ce_configurator->GetDispatcherCell();
	assert(disp_cell);

	size_t visible_records_count = GetVisibleRecordsCount();
	size_t active_record = disp_cell->GetActiveRecord();
	size_t active_field = disp_cell->GetActiveField();

	size_t records_count = GetRecordsCount();
	size_t fields_count = GetFieldsCount();

	if (records_count == 0) return;

	size_t new_record = active_record;
	size_t new_field = active_field;
	bool Key = false;

	switch (eve->GetKey()) {
		case X_VKEY_UP:
			new_record = active_record ? active_record - 1 : 0;
			Key = true;
		break;

		case X_VKEY_DOWN:
			new_record = active_record + 1 >= records_count ? \
							records_count - 1 : active_record + 1;
			Key = true;
		break;

		case X_VKEY_PGUP:
			new_record = (active_record >= visible_records_count) ? \
							active_record - visible_records_count : 0;
			Key = true;
		break;

		case X_VKEY_TAB:
		case X_VKEY_RIGHT:
			new_field = active_field + 1 >= fields_count ? \
							fields_count - 1 : active_field + 1;
			Key = true;
		break;

		case X_VKEY_LEFT:
			new_field = active_field ? active_field - 1 : 0;
			Key = true;
		break;

		case X_VKEY_PGDOWN:
			new_record = (active_record + visible_records_count) >= records_count ? \
							records_count - 1 : active_record + visible_records_count;
			Key = true;
		break;

		case X_VKEY_ENTER:
			disp_cell->SetFocus();
	}

	if (Key && (new_record != active_record || \
				new_field != active_field)) {
		
		disp_cell->OnClick(new_field, new_record);

		if(new_record != active_record)
			FocusOnRecord(new_record);

		if (new_field != active_field)
			FocusOnField(new_field);

		Invalidate(nullptr, false);
	}
}

CEditableGrid::~CEditableGrid(){

	if (field_widgets_collection)
		delete field_widgets_collection;
}

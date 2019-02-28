#include "EditableGrid.h"
#include "DispatcherCell.h"
#include "MultipleCellWidget.h"
#include "EditableCellWidget.h"

class CEConfigurator : public IConfigurator {
	XWindow *parent;
	std::shared_ptr<IGridEventsHandler> events_handler;
	std::shared_ptr<CGridTableProxy> table_proxy;

	CDispatcherCell *disp_cell;
	CMultipleCellWidget *field_widgets_collection;
public:
	CEConfigurator(XWindow *parent_, \
					std::shared_ptr<IGridEventsHandler> events_handler_, \
					std::shared_ptr<CGridTableProxy> table_proxy_, \
					CMultipleCellWidget *field_widgets_collection_) : \
						parent(parent_), events_handler(events_handler_), \
						table_proxy(table_proxy_), \
						disp_cell(nullptr), \
						field_widgets_collection(field_widgets_collection_) { }

	void Configure(CGridLine &line) override { }

	void Configure(CTextCell &text_cell) override {	}

	void Configure(CDispatcherCell &disp_cell_) override {

		this->disp_cell = &disp_cell_;
		IGridCellWidget *cell_widget = field_widgets_collection;
		this->disp_cell->SetParameters(parent, events_handler, cell_widget, table_proxy);
	}

	inline CDispatcherCell *GetDispatcherCell() const {

		return disp_cell;
	}

	virtual ~CEConfigurator() { }
};

//**************************************************************

CEditableGrid::CEditableGrid(std::unique_ptr<IGridEventsHandler> events_handler_) : \
									CGrid(), disp_cell(nullptr), \
									field_widgets_collection(nullptr), \
									events_handler(std::move(events_handler_)) {

	assert(events_handler);
	field_widgets_collection = new CMultipleCellWidget();
	field_widgets_collection->SetDefaultWidget(new CEditableCellWidget());
}

CEditableGrid::CEditableGrid(CEditableGrid &&obj) : CGrid(std::move(obj)){

	disp_cell = obj.disp_cell;
	field_widgets_collection = obj.field_widgets_collection;
	events_handler = std::move(obj.events_handler);

	obj.disp_cell = nullptr;
	obj.field_widgets_collection = nullptr;
}

CEditableGrid &CEditableGrid::operator=(CEditableGrid &&obj) {

	CGrid::operator=(std::move(obj));

	disp_cell = obj.disp_cell;
	field_widgets_collection = obj.field_widgets_collection;
	events_handler = std::move(obj.events_handler);

	obj.disp_cell = nullptr;
	obj.field_widgets_collection = nullptr;

	return *this;
}

LayoutObjects CEditableGrid::CreateLayoutObjects(const int kind_of_layout) {

	return CreateLayoutObjectsHelper<CDispatcherCell>(kind_of_layout);
}

void CEditableGrid::OnWindowCreate() {

	CEConfigurator configurator(this, events_handler, data_table_proxy, \
								field_widgets_collection);

	AcceptConfiguratorOnCells(&configurator);
	disp_cell = configurator.GetDispatcherCell();

	Connect(EVT_KEYDOWN, this, &CEditableGrid::OnKeyPress);
}

void CEditableGrid::SetWidgetForField(const size_t field, IGridCellWidget *field_widget) {

	assert(field_widget);
	assert(!IsCreated());
	field_widgets_collection->AddCellWidget(field, field_widget);
}

void CEditableGrid::OnKeyPress(XKeyboardEvent *eve) {
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

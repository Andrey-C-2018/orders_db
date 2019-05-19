#include "DbGrid.h"
#include <grid/TextCell.h>
#include <editable_grid/IGridEventsHandler.h>

class CDbGridEventsHandler final : public IGridEventsHandler {

	std::shared_ptr<CDbTable> db_table;
	size_t &active_record_no;

public:

	CDbGridEventsHandler(std::shared_ptr<CDbTable> db_table_, size_t &active_record_no_) :
								db_table(db_table_), active_record_no(active_record_no_) { }

	bool OnCellChanged(IGridCellWidget *cell_widget) override {

		return true;
	}

	bool OnCellChangedIndirectly(IGridCellWidget *cell_widget) override {

		return true;
	}

	void OnActiveCellLocationChanged(const size_t new_field, \
										const size_t new_record) override {

		active_record_no = new_record;
		db_table->setCurrentRecordNo(new_record);
	}

};

//**************************************************************

CDbGrid::CDbGrid(std::shared_ptr<CDbTable> db_table_) : \
					dbgrid_event_handler(std::make_shared<CDbGridEventsHandler>(\
														db_table_, active_record_no)), \
					active_record_no(0), \
					db_table(db_table_), pointer_brush(0, 255, 0) {

	SetEventHandler(dbgrid_event_handler);
	Init(db_table, LAYOUT_FIELD);
}

void CDbGrid::DrawLeftPane(XGC &gc) const {

	int left_pane_width = GetLeftPaneSize();
	int height = GetHeight();
	int record_height = GetRecordHeight();
	int y_initial = GetHeadersHeight() + record_height * active_record_no - GetVScrollPos();

	gc.Rectangle(0, 0, (int)(left_pane_width * 0.15F), height);
	XPoint points[5];

	points[0].x = (int)(left_pane_width * 0.15F);
	points[0].y = 0;
	points[1].x = (int)(left_pane_width * 0.15F);
	points[1].y = y_initial + (int)(record_height * 0.15F);
	points[2].x = (int)(left_pane_width * 0.85F);
	points[2].y = y_initial + (int)(record_height * 0.5F);
	points[3].x = left_pane_width;
	points[3].y = y_initial + (int)(record_height * 0.5F);
	points[4].x = left_pane_width;
	points[4].y = 0;
	gc.Polygon(points, 5);

	points[0].x = (int)(left_pane_width * 0.15F);
	points[0].y = height;
	points[1].x = (int)(left_pane_width * 0.15F);
	points[1].y = y_initial + (int)(record_height * 0.85F);
	points[2].x = (int)(left_pane_width * 0.85F);
	points[2].y = y_initial + (int)(record_height * 0.5F);
	points[3].x = left_pane_width;
	points[3].y = y_initial + (int)(record_height * 0.5F);
	points[4].x = left_pane_width;
	points[4].y = height;
	gc.Polygon(points, 5);

	auto old_pen = gc.SelectObject(pointer_brush);
	points[0].x = (int)(left_pane_width * 0.15F);
	points[0].y = y_initial + (int)(record_height * 0.15F);
	points[1].x = (int)(left_pane_width * 0.15F);
	points[1].y = y_initial + (int)(record_height * 0.85F);
	points[2].x = (int)(left_pane_width * 0.85F);
	points[2].y = y_initial + (int)(record_height * 0.5F);
	gc.Polygon(points, 3);

	gc.SelectObject(old_pen);
}

void CDbGrid::SetWidgetForFieldByName(const char *field_name, IGridCellWidget *field_widget) {

	size_t field = db_table->getQuery().getMetaInfo().getFieldIndexByName(field_name);
	CEditableGrid::SetWidgetForField(field, field_widget);
}

CDbGrid::~CDbGrid(){ }

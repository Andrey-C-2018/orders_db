#include "DbGrid.h"
#include <db/DbException.h>
#include <grid/TextCell.h>

CDbGrid::CDbGrid(const bool readonly, std::shared_ptr<CDbTable> db_table_) : CEditableGrid(readonly), \
					dbgrid_event_handler(std::make_shared<CDbGridEventsHandler>(db_table_)), \
					db_table(db_table_), pointer_brush(0, 255, 0) {

	SetEventHandler(dbgrid_event_handler);
	Init(db_table, LAYOUT_FIELD);
}

CDbGrid::CDbGrid(const bool readonly, std::shared_ptr<CDbTable> db_table_, \
					std::shared_ptr<CDbGridEventsHandler> dbgrid_evt_handler) : CEditableGrid(readonly), \
				db_table(db_table_), dbgrid_event_handler(dbgrid_evt_handler), \
				pointer_brush(0, 255, 0) {

	assert(dbgrid_event_handler);
	SetEventHandler(dbgrid_event_handler);
	Init(db_table, LAYOUT_FIELD);
}

void CDbGrid::DrawLeftPane(XGC &gc) const {

	int left_pane_width = GetLeftPaneSize();
	int height = GetHeight();
	int record_height = GetRecordHeight();
	int active_record_no = (int)db_table->getCurrentRecordNo();
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

	size_t field = db_table->getMetaInfo().getFieldIndexByName(field_name);
	CEditableGrid::SetWidgetForField(field, field_widget);
}

CDbGrid::~CDbGrid(){ }

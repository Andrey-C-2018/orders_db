#pragma once
#pragma once
#include <editable_grid/EditableGrid.h>
#include <db_ext/DbTable.h>

class CDbGridEventsHandler;

class CDbGrid :	public CEditableGrid{
	std::shared_ptr<CDbTable> db_table;
	size_t active_record_no;
	std::shared_ptr<CDbGridEventsHandler> dbgrid_event_handler;
	XBrush pointer_brush;

	void DrawLeftPane(XGC &gc) const override;
public:
	CDbGrid(std::shared_ptr<CDbTable> db_table_);

	inline std::shared_ptr<CDbTable> getDbTable() { return db_table; }
	void SetWidgetForFieldByName(const char *field_name, IGridCellWidget *field_widget);

	virtual ~CDbGrid();
};


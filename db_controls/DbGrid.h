#pragma once
#include <editable_grid/EditableGrid.h>
#include <db_ext/DbTable.h>
#include "DbGridEventsHandler.h"

class CDbGrid :	public CEditableGrid{
	std::shared_ptr<CDbTable> db_table;
	std::shared_ptr<CDbGridEventsHandler> dbgrid_event_handler;
	XBrush pointer_brush;

	void DrawLeftPane(XGC &gc) const override;
public:
	CDbGrid(const bool readonly, std::shared_ptr<CDbTable> db_table_);
	CDbGrid(const bool readonly, std::shared_ptr<CDbTable> db_table_, \
			std::shared_ptr<CDbGridEventsHandler> dbgrid_evt_handler);

	CDbGrid(const CDbGrid &obj) = delete;
	CDbGrid(CDbGrid &&obj) = default;
	CDbGrid &operator=(CDbGrid &&obj) = default;
	CDbGrid &operator=(const CDbGrid &obj) = delete;

	inline std::shared_ptr<CDbTable> getDbTable() { return db_table; }
	void SetWidgetForFieldByName(const char *field_name, IGridCellWidget *field_widget);

	virtual ~CDbGrid();
};


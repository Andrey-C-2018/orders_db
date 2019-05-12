#pragma once
#pragma once
#include <editable_grid/EditableGrid.h>
#include <db_ext/DbTable.h>

class CDbGrid :	public CEditableGrid{
	std::shared_ptr<CDbTable> db_table;

public:
	CDbGrid(std::shared_ptr<CDbTable> db_table_);

	inline std::shared_ptr<CDbTable> getDbTable() { return db_table; }
	void SetWidgetForField(const char *field_name, IGridCellWidget *field_widget);

	virtual ~CDbGrid();
};


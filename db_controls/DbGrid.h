#pragma once
#pragma once
#include <grid/EditableGrid.h>
#include <db_ext/DbTable.h>

class CDbGrid :	public CEditableGrid{
	std::shared_ptr<CDbTable> db_table;
public:
	CDbGrid(std::shared_ptr<CDbTable> db_table_);
	virtual ~CDbGrid();
};


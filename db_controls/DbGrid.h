#pragma once
#pragma once
#include <grid/EditableGrid.h>
#include <db_ext/DbTable.h>

class CDbGrid :	public CEditableGrid{
	std::shared_ptr<CDbTable> db_table;

	//LayoutObjects CreateLayoutObjects(const int kind_of_layout) override;
public:
	CDbGrid(std::shared_ptr<CDbTable> db_table_);
	inline std::shared_ptr<CDbTable> getDbTable() { return db_table; }
	virtual ~CDbGrid();
};

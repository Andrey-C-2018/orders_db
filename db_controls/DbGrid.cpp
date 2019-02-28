#include "DbGrid.h"
#include <grid/TextCell.h>
#include <grid/IGridEventsHandler.h>

class CDbGridEventsHandler final : public IGridEventsHandler {

	std::shared_ptr<CDbTable> db_table;
public:

	CDbGridEventsHandler(std::shared_ptr<CDbTable> db_table_) :
								db_table(db_table_) { }

	bool OnCellChanged(IGridCellWidget *cell_widget) override {

		return true;
	}

	bool OnCellChangedIndirectly(IGridCellWidget *cell_widget) override {

		return true;
	}

	void OnActiveCellLocationChanged(const size_t new_field, \
										const size_t new_record) override {

		db_table->setCurrentRecordNo(new_record);
	}
};

//**************************************************************

CDbGrid::CDbGrid(std::shared_ptr<CDbTable> db_table_) : \
					CEditableGrid(std::unique_ptr<IGridEventsHandler>(new CDbGridEventsHandler(db_table_))), \
					db_table(db_table_) {

	Init(db_table, LAYOUT_FIELD);
}

CDbGrid::~CDbGrid(){ }

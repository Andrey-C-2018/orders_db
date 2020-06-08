#include <db_ext/DbTable.h>
#include <db/DbException.h>
#include <xwindows/platform_specific.h>
#include "DbGridEventsHandler.h"

CDbGridEventsHandler::CDbGridEventsHandler(std::shared_ptr<CDbTable> db_table_) : \
											db_table(db_table_), \
											active_field_no(0), active_record_no(0) { }

void CDbGridEventsHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	try {
		action.executeAction();
	}
	catch (CDbException &e) {

		ErrorBox(e.what());
	}
}

void CDbGridEventsHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) {

	try {
		action.executeAction();
	}
	catch (CDbException &e) {

		ErrorBox(e.what());
	}
}

void CDbGridEventsHandler::OnActiveCellLocationChanged(const size_t new_field, \
														const size_t new_record) {

	active_field_no = new_field;
	active_record_no = new_record;
	db_table->setCurrentRecordNo(new_record);
}

CDbGridEventsHandler::~CDbGridEventsHandler() { }

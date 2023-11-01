#include "AdvocatsGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>

CAdvocatsGridEvtHandler::CAdvocatsGridEvtHandler(std::shared_ptr<CDbTable> db_table) : \
												CDbGridEventsHandler(db_table) { }

void CAdvocatsGridEvtHandler::OnCellChangedCommon(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) const {
	auto label = cell_widget->GetLabel();

	try {
		if(cell_widget->Validate(label)) 
			action.executeAction(label);
	}
	catch (CDbException &e) {

		ErrorBox(e.what());
	}
}

void CAdvocatsGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

void CAdvocatsGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
														IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

CAdvocatsGridEvtHandler::~CAdvocatsGridEvtHandler() { }

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
		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring err_str = _T("Цей ID уже використовується: ");
			err_str += label.str;
			ErrorBox(err_str.c_str());
		}
		else
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

#include "AdvocatsGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>

CAdvocatsGridEvtHandler::CAdvocatsGridEvtHandler(std::shared_ptr<CDbTable> db_table) : \
												CDbGridEventsHandler(db_table) { }

void CAdvocatsGridEvtHandler::OnCellChanged(CDbException &e, \
											IGridCellWidget *cell_widget) const {

	if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
		Tstring err_str = _T("Цей ID уже використовується: ");
		err_str += cell_widget->GetLabel().str;
		ErrorBox(err_str.c_str());
	}
	else
		ErrorBox(e.what());
}

void CAdvocatsGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	try {
		action.executeAction();
	}
	catch (CDbException &e) {
		OnCellChanged(e, cell_widget);
	}
}

void CAdvocatsGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
														IOnCellChangedAction &action) {

	try {
		action.executeAction();
	}
	catch (CDbException &e) {
		OnCellChanged(e, cell_widget);
	}
}

CAdvocatsGridEvtHandler::~CAdvocatsGridEvtHandler() { }

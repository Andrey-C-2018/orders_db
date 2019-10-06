#include "PaymentsGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>

CPaymentsGridEvtHandler::CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table) : \
													CDbGridEventsHandler(db_table) { }

void CPaymentsGridEvtHandler::OnCellChangedCommon(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) const {
													auto label = cell_widget->GetLabel();

	try {
		if (cell_widget->Validate(label))
			action.executeAction(label);
	}
	catch (CDbException &e) {
		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring err_str = _T("÷€ стад≥€ уже ≥снуЇ: ");
			err_str += label.str;
			ErrorBox(err_str.c_str());
		}
		else
			ErrorBox(e.what());
	}
}

void CPaymentsGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

void CPaymentsGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

CPaymentsGridEvtHandler::~CPaymentsGridEvtHandler() { }

#include "PaymentsGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>

CPaymentsGridEvtHandler::CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
													std::shared_ptr<CPaymentsConstraints> constraints_) : \
												CDbGridEventsHandler(db_table), \
												constraints(constraints_) { }

void CPaymentsGridEvtHandler::OnCellChangedCommon(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) const {
	if (constraints->wrong_zone) {
		ErrorBox(_T("Неможливо змінити цю стадію, оскільки вона належить до іншого центру"));
		return;
	}
	else if (constraints->old_stage_locked) {
		ErrorBox(_T("Неможливо змінити цю стадію, оскільки вона належить до минулого періоду"));
		return;
	}

	auto label = cell_widget->GetLabel();
	try {
		if (cell_widget->Validate(label))
			action.executeAction(label);
	}
	catch (CDbException &e) {
		if (e.GetErrorCode() == CDbException::E_DB_PRIMARY_KEY_DUPLICATE) {
			Tstring err_str = _T("Ця стадія уже існує: ");
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

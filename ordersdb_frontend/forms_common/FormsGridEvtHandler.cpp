#include "FormsGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>
#include "Constraints.h"

CFormsGridEvtHandler::CFormsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<const CPaymentsConstraints> constraints_) : \
				CDbGridEventsHandler(db_table), \
				constraints(constraints_) { }

void CFormsGridEvtHandler::ProceedWithChanging(IGridCellWidget *cell_widget, \
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

CFormsGridEvtHandler::~CFormsGridEvtHandler() { }

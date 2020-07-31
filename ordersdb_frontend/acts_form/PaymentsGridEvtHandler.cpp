#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>
#include "PaymentsGridEvtHandler.h"

CPaymentsGridEvtHandler::CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<CPaymentsConstraints> constraints_) : \
					CFormsGridEvtHandler(db_table, constraints_) { }

void CPaymentsGridEvtHandler::OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	if (constraints->wrong_zone) {
		ErrorBox(_T("Неможливо змінити цю стадію, оскільки вона належить до іншого центру"));
		return;
	}
	else if (constraints->old_stage_locked && !changesAllowed(getActiveField())) {
		ErrorBox(_T("Неможливо змінити цю стадію, оскільки вона належить до минулого періоду"));
		return;
	}

	ProceedWithChanging(cell_widget, action);
}

void CPaymentsGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	OnSearchFormCellChanged(cell_widget, action);
}

void CPaymentsGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) {

	OnSearchFormCellChanged(cell_widget, action);
}

CPaymentsGridEvtHandler::~CPaymentsGridEvtHandler() { }

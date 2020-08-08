#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>
#include <forms_common/Messages.h>
#include "PaymentsGridEvtHandler.h"

CPaymentsGridEvtHandler::CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<CPaymentsConstraints> constraints_) : \
					CFormsGridEvtHandler(db_table, constraints_) { }

void CPaymentsGridEvtHandler::OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	if (constraints->wrong_zone) {
		ErrorBox(E_WRONG_ZONE);
		return;
	}
	else if (constraints->old_stage_locked && !changesAllowed(getActiveField())) {
		ErrorBox(E_OLD_STAGE);
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

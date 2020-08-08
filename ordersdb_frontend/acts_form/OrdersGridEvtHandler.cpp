#include "OrdersGridEvtHandler.h"
#include <db/DbException.h>
#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>
#include <forms_common/Messages.h>

COrdersGridEvtHandler::COrdersGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
									std::shared_ptr<CPaymentsConstraints> constraints_) : \
							CDbGridEventsHandler(db_table), \
							constraints(constraints_) { }

void COrdersGridEvtHandler::OnCellChangedCommon(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {
	
	if (constraints->old_order_locked) {
		ErrorBox(E_OLD_ORDER);
		return;
	}
	
	auto label = cell_widget->GetLabel();

	try {
		if (cell_widget->Validate(label))
			action.executeAction(label);
	}
	catch (CDbException &e) {
		assert(e.GetErrorCode() != CDbException::E_DB_PRIMARY_KEY_DUPLICATE);
		ErrorBox(e.what());
	}
}

void COrdersGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

void COrdersGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) {

	OnCellChangedCommon(cell_widget, action);
}

COrdersGridEvtHandler::~COrdersGridEvtHandler() { }
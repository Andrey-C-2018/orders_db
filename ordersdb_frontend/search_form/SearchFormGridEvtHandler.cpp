#include "SearchFormGridEvtHandler.h"
#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>

CSearchFormGridEvtHandler::CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<CPaymentsConstraints> constraints_, \
							std::set<size_t> orders_fields_indexes_) : \
				CPaymentsGridEvtHandler(db_table, constraints_), \
				orders_fields_indexes(std::move(orders_fields_indexes_)), \
				deny_changes_to_orders(true) { 

	assert(!orders_fields_indexes.empty());
}

void CSearchFormGridEvtHandler::OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	if (deny_changes_to_orders && ordersCellChanged() && \
		constraints->old_order_locked) {

		ErrorBox(_T("Неможливо змінити це доручення, оскільки воно було додане більше року тому"));
		return;
	}
	OnCellChangedCommon(cell_widget, action);
}

void CSearchFormGridEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	OnSearchFormCellChanged(cell_widget, action);
}

void CSearchFormGridEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
													IOnCellChangedAction &action) {

	OnSearchFormCellChanged(cell_widget, action);
}

CSearchFormGridEvtHandler::~CSearchFormGridEvtHandler() { }

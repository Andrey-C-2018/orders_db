#include "SearchFormGridEvtHandler.h"
#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>
#include <forms_common/Messages.h>

void CSearchFormGridEvtHandler::OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	size_t field = this->getActiveField();

	if (constraints->wrong_zone) {
		ErrorBox(E_WRONG_ZONE);
		return;
	}

	auto p_field = orders_fields_indexes.find(field);
	bool orders_field_to_change = p_field != orders_fields_indexes.cend();
	bool constraints_override = changesAllowed(field);

	if (constraints->stage_finalized) {
		ErrorBox(E_FINALIZED_STAGE);
		return;
	}

	if (constraints->old_order_locked && orders_field_to_change && !constraints_override) {
		ErrorBox(E_OLD_ORDER);
		return;
	}

	if (constraints->old_stage_locked && !constraints_override) {
		ErrorBox(E_OLD_STAGE);
		return;
	}

	ProceedWithChanging(cell_widget, action);
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

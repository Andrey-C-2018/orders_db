#include "SearchFormGridEvtHandler.h"
#include <xwindows/platform_specific.h>
#include <forms_common/Constraints.h>
#include <forms_common/Messages.h>

const wchar_t *E_ENTRIES[] = { E_OLD_ORDER, E_OLD_STAGE };
enum E_Indexes {
	E_OLD_ORDER_INDEX = 0, \
	E_OLD_STAGE_INDEX = 1
};

void CSearchFormGridEvtHandler::OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	size_t field = this->getActiveField();

	if (constraints->wrong_zone) {
		ErrorBox(E_WRONG_ZONE);
		return;
	}
	else {
		auto p_field = orders_fields_indexes.find(field);
		bool orders_field = p_field != orders_fields_indexes.cend();

		int msg_index = 0;
		msg_index = (E_OLD_ORDER_INDEX + 1) * \
					(orders_field && constraints->old_order_locked);
		msg_index += (E_OLD_STAGE_INDEX + 1) * \
					(!orders_field && constraints->old_stage_locked);
		msg_index *= !changesAllowed(this->getActiveField());
		
		if (msg_index) {
			ErrorBox(E_ENTRIES[msg_index - 1]);
			return;
		}
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

#include "AdvocatsListEvtHandler.h"
#include <xwindows/platform_specific.h>

const Tchar msg[] = _T("÷€ таблиц€ - т≥льки дл€ читанн€");

CAdvocatsListEvtHandler::CAdvocatsListEvtHandler(std::shared_ptr<CDbTable> db_table) : \
													CDbGridEventsHandler(db_table) { }

void CAdvocatsListEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	_plMessageBox(msg);
}

void CAdvocatsListEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
														IOnCellChangedAction &action) {

	_plMessageBox(msg);
}

CAdvocatsListEvtHandler::~CAdvocatsListEvtHandler() { }

#include "DefendersListEvtHandler.h"
#include <xwindows/platform_specific.h>

const Tchar msg[] = _T("��� ��������� �� ������ ���� ����� ����� �� �����");

DefendersListEvtHandler::DefendersListEvtHandler(std::shared_ptr<CDbTable> db_table) : \
													CDbGridEventsHandler(db_table) { }

void DefendersListEvtHandler::OnCellChanged(IGridCellWidget *cell_widget, \
											IOnCellChangedAction &action) {

	WarningBox(msg);
}

void DefendersListEvtHandler::OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
														IOnCellChangedAction &action) {

	WarningBox(msg);
}

DefendersListEvtHandler::~DefendersListEvtHandler() { }

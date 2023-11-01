#pragma once
#include <db_controls/DbGridEventsHandler.h>

class DefendersListEvtHandler : public CDbGridEventsHandler {

	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) override;
public:
	DefendersListEvtHandler(std::shared_ptr<CDbTable> db_table);
	virtual ~DefendersListEvtHandler();
};


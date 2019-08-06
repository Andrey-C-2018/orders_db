#pragma once
#include <db_controls/DbGridEventsHandler.h>

class CDbException;

class COrdersGridEvtHandler : public CDbGridEventsHandler {
	void OnCellChangedCommon(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) const;

public:
	COrdersGridEvtHandler(std::shared_ptr<CDbTable> db_table);

	COrdersGridEvtHandler(const COrdersGridEvtHandler &obj) = default;
	COrdersGridEvtHandler(COrdersGridEvtHandler &&obj) = default;
	COrdersGridEvtHandler &operator=(const COrdersGridEvtHandler &obj) = default;
	COrdersGridEvtHandler &operator=(COrdersGridEvtHandler &&obj) = default;

	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) override;

	virtual ~COrdersGridEvtHandler();
};

#pragma once
#include <db_controls/DbGridEventsHandler.h>

class CDbException;

class CAdvocatsGridEvtHandler : public CDbGridEventsHandler {
	void OnCellChanged(CDbException &e, IGridCellWidget *cell_widget) const;

public:
	CAdvocatsGridEvtHandler(std::shared_ptr<CDbTable> db_table);

	CAdvocatsGridEvtHandler(const CAdvocatsGridEvtHandler &obj) = default;
	CAdvocatsGridEvtHandler(CAdvocatsGridEvtHandler &&obj) = default;
	CAdvocatsGridEvtHandler &operator=(const CAdvocatsGridEvtHandler &obj) = default;
	CAdvocatsGridEvtHandler &operator=(CAdvocatsGridEvtHandler &&obj) = default;
	
	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) override;
	
	virtual ~CAdvocatsGridEvtHandler();
};


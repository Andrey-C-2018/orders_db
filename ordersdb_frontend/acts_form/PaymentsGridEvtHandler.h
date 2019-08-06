#pragma once
#include <db_controls/DbGridEventsHandler.h>

class CDbException;

class CPaymentsGridEvtHandler : public CDbGridEventsHandler {
	void OnCellChangedCommon(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) const;

public:
	CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table);

	CPaymentsGridEvtHandler(const CPaymentsGridEvtHandler &obj) = default;
	CPaymentsGridEvtHandler(CPaymentsGridEvtHandler &&obj) = default;
	CPaymentsGridEvtHandler &operator=(const CPaymentsGridEvtHandler &obj) = default;
	CPaymentsGridEvtHandler &operator=(CPaymentsGridEvtHandler &&obj) = default;

	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) override;

	virtual ~CPaymentsGridEvtHandler();
};

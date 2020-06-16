#pragma once
#include <db_controls/DbGridEventsHandler.h>

class CDbException;
struct CPaymentsConstraints;

class COrdersGridEvtHandler : public CDbGridEventsHandler {
	std::shared_ptr<CPaymentsConstraints> constraints;

	void OnCellChangedCommon(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) const;

public:
	COrdersGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<CPaymentsConstraints> constraints_);

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

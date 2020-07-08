#pragma once
#include <db_controls/DbGridEventsHandler.h>

struct CPaymentsConstraints;

class CPaymentsGridEvtHandler : public CDbGridEventsHandler {
	
	inline void OnCellChangedCommon(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;

protected:
	std::shared_ptr<const CPaymentsConstraints> constraints;
	void CheckConstraints() const;
	void ProceedWithChanging(IGridCellWidget *cell_widget, \
							IOnCellChangedAction &action) const;

public:
	CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<const CPaymentsConstraints> constraints_);

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

//*****************************************************

void CPaymentsGridEvtHandler::OnCellChangedCommon(IGridCellWidget *cell_widget, \
												IOnCellChangedAction &action) const {

	CheckConstraints();
	ProceedWithChanging(cell_widget, action);
}
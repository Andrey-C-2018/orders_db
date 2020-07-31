#pragma once
#include <forms_common/FormsGridEvtHandler.h>

class CPaymentsGridEvtHandler : public CFormsGridEvtHandler {

	void OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;
public:
	CPaymentsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<CPaymentsConstraints> constraints_);

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

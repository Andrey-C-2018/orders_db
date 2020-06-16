#pragma once
#include <set>
#include <forms_common/PaymentsGridEvtHandler.h>

class CSearchFormGridEvtHandler : public CPaymentsGridEvtHandler {
	std::set<size_t> orders_fields_indexes;
	bool deny_changes_to_orders;

	inline bool ordersCellChanged() const;
	void OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;

public:
	CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								std::set<size_t> orders_fields_indexes_);

	CSearchFormGridEvtHandler(const CSearchFormGridEvtHandler &obj) = default;
	CSearchFormGridEvtHandler(CSearchFormGridEvtHandler &&obj) = default;
	CSearchFormGridEvtHandler &operator=(const CSearchFormGridEvtHandler &obj) = default;
	CSearchFormGridEvtHandler &operator=(CSearchFormGridEvtHandler &&obj) = default;

	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
								IOnCellChangedAction &action) override;

	virtual ~CSearchFormGridEvtHandler();
};

//*****************************************************

bool CSearchFormGridEvtHandler::ordersCellChanged() const {

	size_t index = this->getActiveField();
	return orders_fields_indexes.find(index) != orders_fields_indexes.end();
}
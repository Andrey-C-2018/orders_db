#pragma once
#include <vector>
#include <forms_common/FormsGridEvtHandler.h>

class CSearchFormGridEvtHandler : public CFormsGridEvtHandler {

	std::set<size_t> orders_fields_indexes;
		
	void OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;

public:
	template <size_t AllowedCount> \
	CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const std::vector<size_t> &orders_fields_indexes_, \
								const size_t (&allowed_fields_indexes_)[AllowedCount]);

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

template <size_t AllowedCount> \
CSearchFormGridEvtHandler::CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const std::vector<size_t> &orders_fields_indexes_, \
								const size_t(&allowed_fields_indexes_)[AllowedCount]) : \
					CFormsGridEvtHandler(db_table, constraints_, \
									allowed_fields_indexes_), \
					orders_fields_indexes(orders_fields_indexes_.cbegin(), \
									orders_fields_indexes_.cend()) { }
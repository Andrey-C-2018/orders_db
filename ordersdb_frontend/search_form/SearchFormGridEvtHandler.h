#pragma once
#include <set>
#include <forms_common/PaymentsGridEvtHandler.h>

class CSearchFormGridEvtHandler : public CPaymentsGridEvtHandler {
	struct FieldItem {
		size_t index;
		bool change_allowed;

		inline bool operator<(const FieldItem &obj) const {

			return index < obj.index;
		}
	};
	typedef std::set<FieldItem>::const_iterator FieldItemsIterator;

	std::set<FieldItem> orders_fields_indexes;
		
	inline FieldItemsIterator ordersCellChanged() const;
	void OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;

public:
	template <size_t TotalCount, size_t AllowedCount> \
	CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const size_t(&orders_fields_indexes_)[TotalCount], \
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

template <size_t TotalCount, size_t AllowedCount> \
CSearchFormGridEvtHandler::CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const size_t(&orders_fields_indexes_)[TotalCount], \
								const size_t(&allowed_fields_indexes_)[AllowedCount]) : \
					CPaymentsGridEvtHandler(db_table, constraints_) {

	assert(TotalCount);

	FieldItem item;
	for (size_t i = 0; i < TotalCount; ++i) {
		item.index = orders_fields_indexes_[i];
		item.change_allowed = std::find(allowed_fields_indexes_, \
										allowed_fields_indexes_ + AllowedCount, \
								item.index) != allowed_fields_indexes_ + AllowedCount;
		orders_fields_indexes.insert(item);
	}
}

CSearchFormGridEvtHandler::FieldItemsIterator \
	CSearchFormGridEvtHandler::ordersCellChanged() const {

	FieldItem item;

	item.index = this->getActiveField();
	return orders_fields_indexes.find(item);
}
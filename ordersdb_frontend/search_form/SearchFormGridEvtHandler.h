#pragma once
#include <vector>
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
		
	template <class Container, size_t AllowedCount> \
		inline void initOrdersFieldsIndexes(const Container &orders_fields_indexes_, \
											const size_t TotalCount, \
											const size_t(&allowed_fields_indexes_)[AllowedCount]);
	inline FieldItemsIterator ordersCellChanged() const;
	void OnSearchFormCellChanged(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) const;

public:
	template <size_t TotalCount, size_t AllowedCount> \
	CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const size_t (&orders_fields_indexes_)[TotalCount], \
								const size_t (&allowed_fields_indexes_)[AllowedCount]);

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

template <size_t TotalCount, size_t AllowedCount> \
CSearchFormGridEvtHandler::CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const size_t(&orders_fields_indexes_)[TotalCount], \
								const size_t(&allowed_fields_indexes_)[AllowedCount]) : \
					CPaymentsGridEvtHandler(db_table, constraints_) {

	initOrdersFieldsIndexes(orders_fields_indexes_, TotalCount, allowed_fields_indexes_);
}

template <size_t AllowedCount> \
CSearchFormGridEvtHandler::CSearchFormGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
								std::shared_ptr<CPaymentsConstraints> constraints_, \
								const std::vector<size_t> &orders_fields_indexes_, \
								const size_t(&allowed_fields_indexes_)[AllowedCount]) : \
				CPaymentsGridEvtHandler(db_table, constraints_) {

	initOrdersFieldsIndexes(orders_fields_indexes_, orders_fields_indexes_.size(), \
							allowed_fields_indexes_);
}

template <class Container, size_t AllowedCount> \
void CSearchFormGridEvtHandler::initOrdersFieldsIndexes(const Container &orders_fields_indexes_, \
										const size_t TotalCount, \
										const size_t(&allowed_fields_indexes_)[AllowedCount]) {

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
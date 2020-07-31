#pragma once
#include <set>
#include <db_controls/DbGridEventsHandler.h>

struct CPaymentsConstraints;

class CFormsGridEvtHandler : public CDbGridEventsHandler {
	std::set<size_t> allowed_indexes;

protected:
	std::shared_ptr<const CPaymentsConstraints> constraints;
	void ProceedWithChanging(IGridCellWidget *cell_widget, \
							IOnCellChangedAction &action) const;
	inline bool changesAllowed(const size_t field) const;

public:
	CFormsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<const CPaymentsConstraints> constraints_);

	template <size_t AllowedCount> \
	CFormsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
							std::shared_ptr<const CPaymentsConstraints> constraints_, \
							const size_t(&allowed_fields_indexes_)[AllowedCount]);

	CFormsGridEvtHandler(const CFormsGridEvtHandler &obj) = default;
	CFormsGridEvtHandler(CFormsGridEvtHandler &&obj) = default;
	CFormsGridEvtHandler &operator=(const CFormsGridEvtHandler &obj) = default;
	CFormsGridEvtHandler &operator=(CFormsGridEvtHandler &&obj) = default;

	inline void addAllowedField(const size_t field);

	virtual ~CFormsGridEvtHandler();
};

//*****************************************************

template <size_t AllowedCount> \
CFormsGridEvtHandler::CFormsGridEvtHandler(std::shared_ptr<CDbTable> db_table, \
						std::shared_ptr<const CPaymentsConstraints> constraints_, \
						const size_t(&allowed_fields_indexes_)[AllowedCount]) : \
					CDbGridEventsHandler(db_table), \
					constraints(constraints_), \
					allowed_indexes(allowed_fields_indexes_, \
									allowed_fields_indexes_ + AllowedCount) { }

bool CFormsGridEvtHandler::changesAllowed(const size_t field) const {

	return allowed_indexes.find(field) != allowed_indexes.cend();
}

void CFormsGridEvtHandler::addAllowedField(const size_t field) {

	allowed_indexes.insert(field);
}                                 
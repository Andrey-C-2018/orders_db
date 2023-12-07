#pragma once
#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>
#include "Constraints.h"
#include "CommonRoutines.h"
#include "Constants.h"

class CPaymentsDbTableEvtHandler : public IDbTableEventsHandler {
private:
	std::weak_ptr<const CDbTable> db_table;
	size_t center_index, order_date_index, act_date_index, payment_date_index;
	const size_t this_center;

	const bool lock_old_stages, check_zone, lock_old_orders, lock_finished_stages;
	std::shared_ptr<CPaymentsConstraints> constraints;

	inline CDate getPeriodForDate(CDate date) const;
public:
	CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
								size_t this_center_, const char *center_field_name, \
								bool lock_old_stages_, bool check_zone_, \
								bool lock_old_orders_, bool lock_finished_stages, \
								std::shared_ptr<CPaymentsConstraints> constraints_);

	void OnCurrRecordNoChanged(const size_t new_record) override;
	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;
	void OnTableReset() override;

	void calcConstraintsValues();

	virtual ~CPaymentsDbTableEvtHandler();
};

//*****************************************************

CDate CPaymentsDbTableEvtHandler::getPeriodForDate(CDate date) const {

	const size_t EXTRA_DAYS = 20;
	unsigned month = ((date.getMonth() - 1) / 3 + 1) * 3;

	return CDate(EXTRA_DAYS, month % 12 + 1, date.getYear() + month / 12);
}

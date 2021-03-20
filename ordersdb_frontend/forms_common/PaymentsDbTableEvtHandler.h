#pragma once
#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>
#include "Constraints.h"
#include "CommonRoutines.h"
#include "Constants.h"

class CPaymentsDbTableEvtHandler : public IDbTableEventsHandler {
private:
	std::weak_ptr<const CDbTable> db_table;
	size_t center_index, order_date_index, act_date_index;
	const size_t this_center;
	const bool lock_old_stages, check_zone, lock_old_orders;
	std::shared_ptr<CPaymentsConstraints> constraints;
	size_t prev_record;

	inline CDate getPeriodForDate(CDate date) const;
public:
	CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
								const size_t this_center_, const char *center_field_name, \
								const bool lock_old_stages_, const bool check_zone_, \
								const bool lock_old_orders_, \
								std::shared_ptr<CPaymentsConstraints> constraints_);

	void OnCurrRecordNoChanged(const size_t new_record) override;
	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;
	void OnTableReset() override;

	inline void calcConstraintsValues();

	virtual ~CPaymentsDbTableEvtHandler();
};

//*****************************************************

void CPaymentsDbTableEvtHandler::calcConstraintsValues() {

	auto rs = getDbTablePtr(db_table)->getResultSet();
	if (rs->empty()) return;

	bool is_null;
	if (lock_old_stages) {
		CDate act_date = rs->getDate(act_date_index, is_null);
		CDate period_end = getPeriodForDate(act_date);
		CDate now = CDate::now();

		constraints->old_stage_locked = !is_null && period_end <= now;
	}

	if (check_zone && this_center != NULL_CENTER) {
		int id_center = rs->getInt(center_index, is_null);
		assert(!is_null);
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);

		constraints->wrong_zone = (this_center == REGIONAL && \
			this_center != id_center && order_date >= CDate(1, 1, 2017)) || \
			(this_center != REGIONAL && ((id_center != REGIONAL && \
				this_center != id_center) || id_center == MEDIATION));

		// the local centers have to be able to add accepted acts of the RC
	}

	if (lock_old_orders) {
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);
		CDate now = CDate::now();
		CDate year_later_dt(now.getDay(), now.getMonth(), now.getYear() - 1);
		
		constraints->old_order_locked = order_date < year_later_dt;
	}
}

CDate CPaymentsDbTableEvtHandler::getPeriodForDate(CDate date) const {

	const size_t EXTRA_DAYS = 20;
	unsigned month = ((date.getMonth() - 1) / 3 + 1) * 3;

	return CDate(EXTRA_DAYS, month % 12 + 1, date.getYear() + month / 12);
}

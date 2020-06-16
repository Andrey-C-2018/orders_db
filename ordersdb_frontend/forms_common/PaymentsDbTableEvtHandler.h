#pragma once
#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>
#include "Constraints.h"
#include "CommonRoutines.h"

class CPaymentsDbTableEvtHandler : public IDbTableEventsHandler {
public:
	enum Centers {
		NULL_CENTER = 0,
		REGIONAL = 1
	};

private:
	std::weak_ptr<const CDbTable> db_table;
	size_t center_index, order_date_index, act_date_index;
	const size_t this_center;
	const bool lock_old_stages, check_zone, lock_old_orders;
	std::shared_ptr<CPaymentsConstraints> constraints;
	size_t prev_record;

	inline CDate getLastQuartalEnd() const;
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
		CDate last_q_end = getLastQuartalEnd();

		constraints->old_stage_locked = !is_null && act_date < last_q_end;
	}

	if (check_zone && this_center != NULL_CENTER) {
		int id_center = rs->getInt(center_index, is_null);
		assert(!is_null);
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);

		constraints->wrong_zone = (this_center == REGIONAL && \
			this_center != id_center && order_date >= CDate(1, 1, 2017)) || \
			(this_center != REGIONAL && this_center != id_center);
	}

	if (lock_old_orders) {
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);
		CDate now = CDate::now();
		CDate year_later_dt(now.getDay(), now.getMonth(), now.getYear() - 1);
		
		constraints->old_order_locked = order_date < year_later_dt;
	}
}

CDate CPaymentsDbTableEvtHandler::getLastQuartalEnd() const {

	CDate this_year_beginning = CDate::now();

	const unsigned year = this_year_beginning.getYear();
	const unsigned month = this_year_beginning.getMonth();

	unsigned last_q_month = month % 3 == 0 ? month - 2 : \
								((unsigned)(month / 3)) * 2 + 1;

	return CDate(1, last_q_month, year);
}
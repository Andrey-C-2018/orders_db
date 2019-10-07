#pragma once
#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>
#include "Constraints.h"

class CPaymentsDbTableEvtHandler : public IDbTableEventsHandler {
public:
	enum Centers {
		NULL_CENTER = 0,
		REGIONAL = 1
	};

private:
	std::shared_ptr<const CDbTable> db_table;
	size_t center_index, order_date_index, act_date_index;
	const size_t this_center;
	const bool lock_old_stages;
	std::shared_ptr<CPaymentsConstraints> constraints;
	size_t prev_record;

	inline void OnRecordNoChanged();
public:
	CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
								const size_t this_center_, const bool lock_old_stages_, \
								std::shared_ptr<CPaymentsConstraints> constraints_);

	void OnCurrRecordNoChanged(const size_t new_record) override;
	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;
	void OnTableReset() override;

	virtual ~CPaymentsDbTableEvtHandler();
};

void CPaymentsDbTableEvtHandler::OnRecordNoChanged() {

	auto rs = db_table->getResultSet();
	if (rs->empty()) return;

	bool is_null;
	if (lock_old_stages) {
		CDate act_date = rs->getDate(act_date_index, is_null);
		CDate this_year_begginning = CDate::now();

		const unsigned year = this_year_begginning.getYear();
		this_year_begginning.set(1, 1, year);
		constraints->old_stage_locked = !is_null && act_date < this_year_begginning;
	}

	if (this_center != NULL_CENTER) {
		int id_center = rs->getInt(center_index, is_null);
		assert(!is_null);
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);

		constraints->wrong_zone = (this_center == REGIONAL && \
			this_center != id_center && order_date >= CDate(1, 1, 2017)) || \
			(this_center != REGIONAL && this_center != id_center);
	}
}
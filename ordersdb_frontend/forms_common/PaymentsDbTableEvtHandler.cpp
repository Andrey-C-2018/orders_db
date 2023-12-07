#include "PaymentsDbTableEvtHandler.h"

CPaymentsDbTableEvtHandler::CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
														size_t this_center_, \
														const char *center_field_name, \
														bool lock_old_stages_, \
														bool check_zone_, \
														bool lock_old_orders_, \
														bool lock_finished_stages_, \
														std::shared_ptr<CPaymentsConstraints> constraints_) : \
											db_table(db_table_), \
											this_center(this_center_), \
											lock_old_stages(lock_old_stages_), \
											check_zone(check_zone_), \
											lock_old_orders(lock_old_orders_), \
											lock_finished_stages(lock_finished_stages_), \
											constraints(constraints_) {

	assert(center_field_name);
	assert(db_table_);

	const CMetaInfo &meta_info = db_table_->getMetaInfo();
	center_index = meta_info.getFieldIndexByName(center_field_name);
	order_date_index = meta_info.getFieldIndexByName("order_date");
	act_date_index = meta_info.getFieldIndexByName("act_date");
	payment_date_index = meta_info.getFieldIndexByName("payment_date");
}

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

	if (lock_finished_stages) {
		CDate payment_date = rs->getDate(payment_date_index, is_null);
		constraints->stage_finalized = !is_null;
	}

	if (check_zone && this_center != NULL_CENTER) {
		int id_center = rs->getInt(center_index, is_null);
		assert(!is_null);
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);

		constraints->wrong_zone = (this_center == REGIONAL && \
			this_center != id_center && id_center != MEDIATION && \
			order_date >= CDate(1, 1, 2017)) || \
			(this_center != REGIONAL && this_center != id_center);
	}

	if (lock_old_orders) {
		CDate order_date = rs->getDate(order_date_index, is_null);
		assert(!is_null);
		CDate now = CDate::now();
		CDate year_before_now_dt(now.getDay(), now.getMonth(), now.getYear() - 1);

		constraints->old_order_locked = order_date < year_before_now_dt;
	}
}

void CPaymentsDbTableEvtHandler::OnCurrRecordNoChanged(const size_t new_record) {

	calcConstraintsValues();
}

void CPaymentsDbTableEvtHandler::OnFieldsCountChanged(const size_t new_fields_count) { }

void CPaymentsDbTableEvtHandler::OnRecordsCountChanged(const size_t new_records_count) {

	calcConstraintsValues();
}

void CPaymentsDbTableEvtHandler::OnTableReset() {

	calcConstraintsValues();
}

CPaymentsDbTableEvtHandler::~CPaymentsDbTableEvtHandler() { }

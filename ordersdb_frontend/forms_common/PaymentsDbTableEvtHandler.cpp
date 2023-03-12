#include "PaymentsDbTableEvtHandler.h"

CPaymentsDbTableEvtHandler::CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
														const size_t this_center_, \
														const char *center_field_name, \
														const bool lock_old_stages_, \
														const bool check_zone_, \
														const bool lock_old_orders_, \
														std::shared_ptr<CPaymentsConstraints> constraints_) : \
											db_table(db_table_), \
											this_center(this_center_), \
											lock_old_stages(lock_old_stages_), \
											check_zone(check_zone_), \
											lock_old_orders(lock_old_orders_), \
											constraints(constraints_), \
											prev_record((size_t)-1) {

	assert(center_field_name);
	assert(db_table_);

	const CMetaInfo &meta_info = db_table_->getMetaInfo();
	center_index = meta_info.getFieldIndexByName(center_field_name);
	order_date_index = meta_info.getFieldIndexByName("order_date");
	act_date_index = meta_info.getFieldIndexByName("act_date");
}

void CPaymentsDbTableEvtHandler::OnCurrRecordNoChanged(const size_t new_record) {

	if(prev_record != new_record) calcConstraintsValues();
	prev_record = new_record;
}

void CPaymentsDbTableEvtHandler::OnFieldsCountChanged(const size_t new_fields_count) { }

void CPaymentsDbTableEvtHandler::OnRecordsCountChanged(const size_t new_records_count) {

	calcConstraintsValues();
}

void CPaymentsDbTableEvtHandler::OnTableReset() {

	calcConstraintsValues();
}

CPaymentsDbTableEvtHandler::~CPaymentsDbTableEvtHandler() { }

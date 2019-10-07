#include "PaymentsDbTableEvtHandler.h"

CPaymentsDbTableEvtHandler::CPaymentsDbTableEvtHandler(std::shared_ptr<const CDbTable> db_table_, \
														const size_t this_center_, \
														const bool lock_old_stages_, \
														std::shared_ptr<CPaymentsConstraints> constraints_) : \
															db_table(db_table_), \
															this_center(this_center_), \
															lock_old_stages(lock_old_stages_), \
															constraints(constraints_), \
															prev_record((size_t)-1) {

	const CMetaInfo &meta_info = db_table->getQuery().getMetaInfo();
	center_index = meta_info.getFieldIndexByName("id_center");
	order_date_index = meta_info.getFieldIndexByName("order_date");
	act_date_index = meta_info.getFieldIndexByName("act_date");
}

void CPaymentsDbTableEvtHandler::OnCurrRecordNoChanged(const size_t new_record) {

	if(prev_record != new_record) OnRecordNoChanged();
	prev_record = new_record;
}

void CPaymentsDbTableEvtHandler::OnFieldsCountChanged(const size_t new_fields_count) { }

void CPaymentsDbTableEvtHandler::OnRecordsCountChanged(const size_t new_records_count) {

	OnRecordNoChanged();
}

void CPaymentsDbTableEvtHandler::OnTableReset() {

	OnRecordNoChanged();
}

CPaymentsDbTableEvtHandler::~CPaymentsDbTableEvtHandler() { }

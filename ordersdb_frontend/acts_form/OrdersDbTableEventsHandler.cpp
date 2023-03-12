#include "OrdersDbTableEventsHandler.h"

COrdersDbTableEventsHandler::COrdersDbTableEventsHandler(std::shared_ptr<const CDbTable> master_table_, \
											std::shared_ptr<CDbTable> dependent_table_, \
											size_t(&master_key_)[MASTER_KEY_SIZE], \
											size_t(&dependent_params)[MASTER_KEY_SIZE]) : \
							master_table(master_table_), dependent_table(dependent_table_) {

	::memcpy_s(this->dep_params, sizeof(size_t[MASTER_KEY_SIZE]), \
				dependent_params, sizeof(size_t[MASTER_KEY_SIZE]));

	master_fields[0] = master_table_->getMetaInfo().getField(master_key_[0]);
	master_fields[1] = master_table_->getMetaInfo().getField(master_key_[1]);
	master_fields[2] = master_table_->getMetaInfo().getField(master_key_[2]);
}

void COrdersDbTableEventsHandler::OnCurrRecordNoChanged(const size_t new_record) {

	applyCurrRecordChanges();
}

void COrdersDbTableEventsHandler::OnFieldsCountChanged(const size_t new_fields_count) { }

void COrdersDbTableEventsHandler::OnRecordsCountChanged(const size_t new_records_count) {

	if (new_records_count) {
		applyCurrRecordChanges();
	}
	else {
		auto dependent_table_shptr = getDbTablePtr(dependent_table);
		auto dep_binding_target = dependent_table_shptr->getBindingTarget();

		dep_binding_target->bindValue(0, 0);
		dep_binding_target->bindValue(1, 0);
		dep_binding_target->bindNull(2);
		dependent_table_shptr->reload();
	}
}

void COrdersDbTableEventsHandler::OnTableReset() { }

COrdersDbTableEventsHandler::~COrdersDbTableEventsHandler() { }

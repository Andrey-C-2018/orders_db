#include "DefenderDbTableEvtHandler.h"
#include <db_ext/DbTable.h>
#include <forms_common/CommonRoutines.h>

DefenderDbTableEvtHandler::DefenderDbTableEvtHandler(std::shared_ptr<const CDbTable> master_table_, \
							std::shared_ptr<CDbTable> dependent_table_, \
							const size_t master_field_no_, \
							const size_t dependent_param_no) : \
							master_table(master_table_), dependent_table(dependent_table_), \
							dep_param_no(dependent_param_no) {

	master_field = master_table_->getMetaInfo().getField(master_field_no_);
}

void DefenderDbTableEvtHandler::OnCurrRecordNoChanged(const size_t new_record) {

	auto master_result_set = getDbTablePtr(master_table)->getResultSet();
	auto dep_binding_target = dependent_table->getBindingTarget();

	master_field->getValueAndBindItTo(master_result_set, dep_binding_target, dep_param_no);
	dependent_table->reload();
}

void DefenderDbTableEvtHandler::OnFieldsCountChanged(const size_t new_fields_count) { }

void DefenderDbTableEvtHandler::OnRecordsCountChanged(const size_t new_records_count) { }

void DefenderDbTableEvtHandler::OnTableReset() { }

DefenderDbTableEvtHandler::~DefenderDbTableEvtHandler() { }
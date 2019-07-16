#pragma once
#include <memory>
#include <db_ext/IDbTableEventsHandler.h>
#include <db_ext/DbTable.h>

class COrdersDbTableEventsHandler : public IDbTableEventsHandler {
	enum {
		MASTER_KEY_SIZE = 3
	};

	std::shared_ptr<const CDbTable> master_table;
	std::shared_ptr<CDbTable> dependent_table;
	std::shared_ptr<const IDbField> master_fields[MASTER_KEY_SIZE];
	size_t dep_params[MASTER_KEY_SIZE];

	inline void applyCurrRecordChanges();
public:
	COrdersDbTableEventsHandler(std::shared_ptr<const CDbTable> master_table_, \
								std::shared_ptr<CDbTable> dependent_table_, \
								size_t (&master_key_)[MASTER_KEY_SIZE], \
								size_t(&dependent_params)[MASTER_KEY_SIZE]);

	void OnCurrRecordNoChanged(const size_t new_record) override;
	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;
	void OnTableReset() override;

	virtual ~COrdersDbTableEventsHandler();
};

void COrdersDbTableEventsHandler::applyCurrRecordChanges() {

	auto master_result_set = master_table->getResultSet();
	auto dep_binding_target = dependent_table->getBindingTarget();

	master_fields[0]->getValueAndBindItTo(master_result_set, dep_binding_target, dep_params[0]);
	master_fields[1]->getValueAndBindItTo(master_result_set, dep_binding_target, dep_params[1]);
	master_fields[2]->getValueAndBindItTo(master_result_set, dep_binding_target, dep_params[2]);
	dependent_table->reload();
}
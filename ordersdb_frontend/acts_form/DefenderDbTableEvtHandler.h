#pragma once
#include <memory>
#include <db_ext/IDbTableEventsHandler.h>

class CDbTable;
struct IDbField;

class DefenderDbTableEvtHandler : public IDbTableEventsHandler {
	std::weak_ptr<const CDbTable> master_table;
	std::shared_ptr<CDbTable> dependent_table;
	std::shared_ptr<const IDbField> master_field;
	size_t dep_param_no;

public:
	DefenderDbTableEvtHandler(std::shared_ptr<const CDbTable> master_table_, \
								std::shared_ptr<CDbTable> dependent_table_, \
								const size_t master_field_no_, \
								const size_t dependent_param_no);

	void OnCurrRecordNoChanged(const size_t new_record) override;
	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;
	void OnTableReset() override;

	virtual ~DefenderDbTableEvtHandler();
};


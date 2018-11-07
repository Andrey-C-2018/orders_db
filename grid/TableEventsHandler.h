#pragma once
#include <memory>
#include <table/ITable.h>

class CFieldsProperties;
struct IReloadable;

class CDataTableEventsHandler final : public ITableEventsHandler {
	size_t records_count;

	IReloadable *grid;
	std::shared_ptr<CFieldsProperties> fields_props;
public:
	CDataTableEventsHandler(IReloadable *grid_, \
							std::shared_ptr<CFieldsProperties> fields_props_);

	CDataTableEventsHandler(const CDataTableEventsHandler &obj) = default;
	CDataTableEventsHandler(CDataTableEventsHandler &&obj) = default;
	CDataTableEventsHandler &operator=(const CDataTableEventsHandler &obj) = default;
	CDataTableEventsHandler &operator=(CDataTableEventsHandler &&obj) = default;

	void OnFieldsCountChanged(const size_t new_fields_count) override;
	void OnRecordsCountChanged(const size_t new_records_count) override;

	void OnTableReset() override;

	inline size_t GetFieldsCount() const { return fields_props->size(); }
	inline size_t GetRecordsCount() const { return records_count; }

	virtual ~CDataTableEventsHandler();
};
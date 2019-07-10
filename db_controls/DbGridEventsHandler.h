#pragma once
#include <memory>
#include <editable_grid/IGridEventsHandler.h>

class CDbTable;

class CDbGridEventsHandler : public IGridEventsHandler {

	std::shared_ptr<CDbTable> db_table;
	size_t active_field_no, active_record_no;
	void OnActiveCellLocationChanged(const size_t new_field, \
									const size_t new_record) override final;

public:
	CDbGridEventsHandler(std::shared_ptr<CDbTable> db_table_);

	CDbGridEventsHandler(const CDbGridEventsHandler &obj) = default;
	CDbGridEventsHandler(CDbGridEventsHandler &&obj) = default;
	CDbGridEventsHandler &operator=(const CDbGridEventsHandler &obj) = default;
	CDbGridEventsHandler &operator=(CDbGridEventsHandler &&obj) = default;
	
	void OnCellChanged(IGridCellWidget *cell_widget, \
						IOnCellChangedAction &action) override;

	void OnCellChangedIndirectly(IGridCellWidget *cell_widget, \
									IOnCellChangedAction &action) override;

	inline size_t getActiveField() const { return active_field_no; }
	inline size_t getActiveRecord() const { return active_record_no; }

	virtual ~CDbGridEventsHandler();
};
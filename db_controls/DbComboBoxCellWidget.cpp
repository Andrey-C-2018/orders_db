#include "DbComboBoxCellWidget.h"
#include <editable_grid/ICellEventHandler.h>

CDbComboBoxCellWidget::CDbComboBoxCellWidget(std::shared_ptr<IDbConnection> conn, \
											const size_t field_to_display_, \
											const char *master_table_name, \
											const char *dependent_table_name, \
											std::shared_ptr<CDbTable> dependent_table_) : \
									field_to_display(field_to_display_), \
									updater(conn, master_table_name, \
									dependent_table_name, dependent_table_), \
									dependent_table(dependent_table_){ }

CDbComboBoxCellWidget::CDbComboBoxCellWidget(std::shared_ptr<IDbConnection> conn, \
											const size_t field_to_display_, \
											std::shared_ptr<const IDbResultSet> master_records, \
											std::shared_ptr<const IDbResultSetMetadata> master_metadata, \
											const char *dependent_table_name, \
											std::shared_ptr<CDbTable> dependent_table_) : \
									field_to_display(field_to_display_), \
									updater(conn, master_records, master_metadata, \
									dependent_table_name, dependent_table_), \
									dependent_table(dependent_table_) { }

void CDbComboBoxCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	CComboBoxCellWidget::CreateCellWidget(parent, flags, label, x, y, width, height);

	fillComboBox(updater.getMasterResultSet());
	XComboBox::GetParent()->Connect(EVT_COMMAND, NCODE_COMBOBOX_SELCHANGED, \
									XComboBox::GetId(), this, &CDbComboBoxCellWidget::OnItemChoosed);
}

void CDbComboBoxCellWidget::fillComboBox(std::shared_ptr<const IDbResultSet> master_records) {

	size_t records_count = master_records->getRecordsCount();
	for (size_t i = 0; i < records_count; ++i){
		master_records->gotoRecord(i);
		XComboBox::AddItem(master_records->getWString(field_to_display));
	}
}

void CDbComboBoxCellWidget::AddRelation(const char *master_field, const char *dependent_field) {

	updater.AddRelation(master_field, dependent_field);
}

void CDbComboBoxCellWidget::OnItemChoosed(XCommandEvent *eve) {

	assert(on_indirect_change_handler);
	if (!on_indirect_change_handler->OnCellChangedIndirectly(this))
		return;

	size_t sel_index = XComboBox::GetCurrentSelectionIndex();
	dependent_table->gotoCurrentRecord();
	
	auto stmt = updater.createDepTableUpdateStmt(sel_index);
	dependent_table->executeScalarStmt(stmt);
}

void CDbComboBoxCellWidget::SetOnChangeHandler(XEventHandlerData on_change) { }

void CDbComboBoxCellWidget::SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> handler) {

	on_indirect_change_handler = handler;
}

CDbComboBoxCellWidget::~CDbComboBoxCellWidget() { }

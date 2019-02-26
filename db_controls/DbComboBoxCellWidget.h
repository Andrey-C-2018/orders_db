#pragma once
#include <grid/ComboBoxCellWidget.h>
#include <db_ext/DependentTableUpdater.h>

class CDbComboBoxCellWidget : public CComboBoxCellWidget {
	size_t field_to_display;
	CDependentTableUpdater updater;
	CDelegate on_indirect_change_handler;
	std::shared_ptr<CDbTable> dependent_table;
	
	void fillComboBox(std::shared_ptr<const IDbResultSet> master_records);
	void OnItemChoosed(XCommandEvent *eve);

	CDbComboBoxCellWidget(const CDbComboBoxCellWidget &obj) = delete;
	CDbComboBoxCellWidget &operator=(const CDbComboBoxCellWidget &obj) = delete;
public:
	CDbComboBoxCellWidget(std::shared_ptr<IDbConnection> conn, \
							const size_t field_to_display_, \
							const char *master_table_name, \
							const char *dependent_table_name, \
							std::shared_ptr<CDbTable> dependent_table);
	CDbComboBoxCellWidget(std::shared_ptr<IDbConnection> conn, \
							const size_t field_to_display_, \
							std::shared_ptr<const IDbResultSet> master_records, \
							std::shared_ptr<const IDbResultSetMetadata> master_metadata, \
							const char *dependent_table_name, \
							std::shared_ptr<CDbTable> dependent_table);

	CDbComboBoxCellWidget(CDbComboBoxCellWidget &&obj) = default;
	CDbComboBoxCellWidget &operator=(CDbComboBoxCellWidget &&obj) = default;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void AddRelation(const char *master_field, const char *dependent_field);

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(XEventHandlerData on_indirect_change) override;

	virtual ~CDbComboBoxCellWidget();
};


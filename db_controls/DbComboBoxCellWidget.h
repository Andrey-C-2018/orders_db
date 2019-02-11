#pragma once
#include <grid/ComboBoxCellWidget.h>
#include <db_ext/DependentTableUpdater.h>
#include "IDbTablesRelationsContainer.h"

class CDbComboBoxCellWidget : public CComboBoxCellWidget, public IDbTablesRelationsContainer {
	CDependentTableUpdater updater;

	CDbComboBoxCellWidget(const CDbComboBoxCellWidget &obj) = delete;
	CDbComboBoxCellWidget &operator=(const CDbComboBoxCellWidget &obj) = delete;

	void OnItemChoosed(XCommandEvent *eve);
public:
	CDbComboBoxCellWidget(const char *table_to_get_data_from, const size_t field);
	CDbComboBoxCellWidget(std::shared_ptr<const IDbResultSet> rs_to_get_data_from, \
							const size_t field);

	CDbComboBoxCellWidget(CDbComboBoxCellWidget &&obj) = default;
	CDbComboBoxCellWidget &operator=(CDbComboBoxCellWidget &&obj) = default;

	void SetRelation(const char *master_field, const char *dependent_field) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override;
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override;

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	virtual ~CDbComboBoxCellWidget();
};


#pragma once
#include <grid/ComboBoxCellWidget.h>

class CDbComboBoxCellWidget : public CComboBoxCellWidget {

	CDbComboBoxCellWidget(const CDbComboBoxCellWidget &obj) = delete;
	CDbComboBoxCellWidget &operator=(const CDbComboBoxCellWidget &obj) = delete;
public:
	CDbComboBoxCellWidget();

	CDbComboBoxCellWidget(CDbComboBoxCellWidget &&obj) = default;
	CDbComboBoxCellWidget &operator=(CDbComboBoxCellWidget &&obj) = default;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override;
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override;

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	virtual ~CDbComboBoxCellWidget();
};


#pragma once
#include "IGridCellWidget.h"
#include <xwindows/XComboBox.h>

class CComboBoxCellWidget : public XComboBox, public IGridCellWidget {
	enum {
		DROPDOWN_PART_SIZE = 100
	};

	CComboBoxCellWidget(const CComboBoxCellWidget &obj) = delete;
	CComboBoxCellWidget &operator=(const CComboBoxCellWidget &obj) = delete;
public:
	CComboBoxCellWidget();

	CComboBoxCellWidget(CComboBoxCellWidget &&obj) = default;
	CComboBoxCellWidget &operator=(CComboBoxCellWidget &&obj) = default;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> ignored);
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override;
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override;

	void SetCurrentField(const size_t field) override;

	void Show() override;
	void Hide() override;
	void MoveWindow(const int x, const int y, \
					const int width, const int height) override;
	void SetFocus() override;

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;
	virtual ~CComboBoxCellWidget();
};


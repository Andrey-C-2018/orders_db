#pragma once
#include "IGridCellWidget.h"
#include <xwindows/XComboBox.h>

class CComboBoxCellWidget : public XComboBox, public IGridCellWidget {
	enum {
		DROPDOWN_PART_SIZE = 200
	};
	const bool initial_null_item;

public:
	CComboBoxCellWidget();
	CComboBoxCellWidget(bool add_null_item);

	CComboBoxCellWidget(const CComboBoxCellWidget &obj) = delete;
	CComboBoxCellWidget(CComboBoxCellWidget &&obj) = default;
	CComboBoxCellWidget &operator=(const CComboBoxCellWidget &obj) = delete;
	CComboBoxCellWidget &operator=(CComboBoxCellWidget &&obj) = default;

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> ignored);
	void SetOnGetFocusHandler(XEventHandlerData on_get_focus) override;
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override;
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override;

	void SetCurrentField(const size_t field) override;

	void Show() override;
	void Hide() override;
	void MoveWindow(const int x, const int y, \
					const int width, const int height) override;
	void SetFocus() override;
	bool HasFocus() const override;
	void SelectAll() override { }

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;

	bool Validate() const override { return true; }
	bool Validate(ImmutableString<Tchar> label) const override { return true; }

	virtual ~CComboBoxCellWidget();
};


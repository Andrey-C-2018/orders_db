#pragma once
#include "IGridCellWidget.h"
#include <xwindows/XEdit.h>

class CEditableCellWidget : public XEdit, public IGridCellWidget {
	const bool def_readonly;
public:
	CEditableCellWidget();
	CEditableCellWidget(const bool def_readonly_);

	CEditableCellWidget(const CEditableCellWidget &obj) = delete;
	CEditableCellWidget(CEditableCellWidget &&obj) = default;
	CEditableCellWidget &operator=(CEditableCellWidget &&obj) = default;
	CEditableCellWidget &operator=(const CEditableCellWidget &obj) = delete;

	void CreateCellWidget(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> ignored) override;
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

	ImmutableString<Tchar> GetLabel() override;
	ImmutableString<Tchar> GetLabel(std::vector<Tchar> &label) const;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;

	bool Validate() const override { return true; }
	bool Validate(ImmutableString<Tchar> label) const override { return true; }

	virtual ~CEditableCellWidget();
};

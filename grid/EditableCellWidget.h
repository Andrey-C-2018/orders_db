#pragma once
#include "IGridCellWidget.h"
#include "xwindows/XEdit.h"

class CEditableCellWidget final : public XEdit, public IGridCellWidget {
	
	CEditableCellWidget(const CEditableCellWidget &obj) = delete;
	CEditableCellWidget &operator=(const CEditableCellWidget &obj) = delete;
public:
	CEditableCellWidget();

	CEditableCellWidget(CEditableCellWidget &&obj) = default;
	CEditableCellWidget &operator=(CEditableCellWidget &&obj) = default;

	void CreateCellWidget(XWindow *parent, const int flags, \
				const Tchar *label, \
				const int x, const int y, \
				const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> ignored) override;
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

	virtual ~CEditableCellWidget();

	DECLARE_EVENT_OVERRIDE
};


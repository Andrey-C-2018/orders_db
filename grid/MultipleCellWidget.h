#pragma once
#include <map>
#include "IGridCellWidget.h"

class CMultipleCellWidget : public IGridCellWidget {

	IGridCellWidget *curr_widget, *default_widget;
	bool is_created;
	std::map<size_t, IGridCellWidget *> widgets;
public:
	CMultipleCellWidget();

	void SetCurrentField(const size_t field);

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
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

	virtual ~CMultipleCellWidget();
};


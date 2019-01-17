#pragma once
#include <map>
#include "IGridCellWidget.h"
#include <xwindows/XEdit.h>

class CMultipleCellWidget : public XEdit, public IGridCellWidget {
	enum {
		NOT_CREATED = -1
	};

	struct CCellWidget {
		IGridCellWidget *widget;
		int id;
	};

	std::map<size_t, CCellWidget> widgets;
	IGridCellWidget *curr_widget;
	size_t curr_field;
	int flags;
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

	void AddCellWidget(const size_t field, IGridCellWidget *widget);
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


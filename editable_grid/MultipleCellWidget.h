#pragma once
#include <map>
#include "IGridCellWidget.h"
#include <xwindows/XEventHandler.h>

class CMultipleCellWidget : public IGridCellWidget {
	enum {
		NOT_CREATED = -1
	};

	struct CCellWidget {
		IGridCellWidget *widget;
		int id;
	};

	std::map<size_t, CCellWidget> widgets;
	IGridCellWidget *curr_widget, *default_widget;
	int default_widget_id;
	size_t curr_field;

	XWindow *parent;
	int flags;
	XEventHandlerData on_change_handler, \
						on_get_focus_handler, \
						on_loose_focus_handler, \
						on_key_press_handler;
	std::shared_ptr<ICellEventHandler> on_indirect_change_handler;

public:
	CMultipleCellWidget();

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
	void SetOnIndirectChangeHandler(std::shared_ptr<ICellEventHandler> handler) override;
	void SetOnGetFocusHandler(XEventHandlerData on_get_focus) override;
	void SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) override;
	void SetOnKeyPressHandler(XEventHandlerData on_key_press) override;

	void SetDefaultWidget(IGridCellWidget *widget);
	void AddCellWidget(const size_t field, IGridCellWidget *widget);
	void SetCurrentField(const size_t field) override;
	
	void Show() override;
	void Hide() override;
	void MoveWindow(const int x, const int y, \
					const int width, const int height) override;
	void SetFocus() override;
	bool HasFocus() const override;

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CMultipleCellWidget();
};

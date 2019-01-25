#pragma once
#include <map>
#include "IGridCellWidget.h"
#include <xwindows/XEdit.h>

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
						on_loose_focus_handler, \
						on_key_press_handler;

	typedef void (IGridCellWidget::*PEventFunc)(XEventHandlerData);
	inline void SetEventHandler(XEventHandlerData &on_event_this, \
								PEventFunc event_func, XEventHandlerData on_event);
public:
	CMultipleCellWidget();

	void CreateCellWidget(XWindow *parent, const int flags, \
							const Tchar *label, \
							const int x, const int y, \
							const int width, const int height) override;

	void SetOnChangeHandler(XEventHandlerData on_change) override;
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

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;

	virtual ~CMultipleCellWidget();
};

void CMultipleCellWidget::SetEventHandler(XEventHandlerData &on_event_this, \
											PEventFunc event_func, \
											XEventHandlerData on_event) {

	assert(default_widget);
	on_event_this = on_event;

	if (default_widget_id != NOT_CREATED) {
		(default_widget->*(event_func))(std::move(on_event));

		for (auto &widget : widgets)
			(widget.second.widget->*(event_func))(on_event_this);
	}
}
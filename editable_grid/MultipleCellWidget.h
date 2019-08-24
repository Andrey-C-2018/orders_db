#pragma once
#include <map>
#include "IGridCellWidget.h"
#include <xwindows/XEventHandler.h>

class CMultipleCellWidget : public IGridCellWidget {
	enum {
		NOT_CREATED = -1, 
		DEF_WIDGETS_COUNT = 10
	};

	struct CCellWidget {
		IGridCellWidget *widget;
		int id;
		size_t ref_counter;

		inline bool operator==(const CCellWidget &obj) const {

			return widget == obj.widget;
		}
	};

	std::vector<CCellWidget> widgets;
	size_t widgets_count;
	std::map<size_t, size_t> assignments;
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

	inline void SetEvtHandlers(IGridCellWidget *widget);

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

	inline const IGridCellWidget *GetWidget(const size_t field) const;
	inline const IGridCellWidget *GetCurrentWidget() const;
	inline size_t GetWidgetsCount() const;
	inline size_t GetAssignmentsCount() const;
	void SetDefaultWidget(IGridCellWidget *widget);
	void AddCellWidget(const size_t field, IGridCellWidget *widget);
	void SetCurrentField(const size_t field) override;
	
	void Show() override;
	void Hide() override;
	void MoveWindow(const int x, const int y, \
					const int width, const int height) override;
	void SetFocus() override;
	bool HasFocus() const override;
	void SelectAll() override;

	ImmutableString<Tchar> GetLabel() override;
	void SetLabel(ImmutableString<Tchar> label) override;

	int GetId() const override;

	bool Validate() const override;
	bool Validate(ImmutableString<Tchar> label) const override;

	virtual ~CMultipleCellWidget();
};

//*****************************************************

void CMultipleCellWidget::SetEvtHandlers(IGridCellWidget *widget) {

	if (!this->on_change_handler.empty())
		widget->SetOnChangeHandler(this->on_change_handler);
	if (!this->on_get_focus_handler.empty())
		widget->SetOnGetFocusHandler(this->on_get_focus_handler);
	if (!this->on_loose_focus_handler.empty())
		widget->SetOnLooseFocusHandler(this->on_loose_focus_handler);
	if (!this->on_key_press_handler.empty())
		widget->SetOnKeyPressHandler(this->on_key_press_handler);
}

const IGridCellWidget *CMultipleCellWidget::GetWidget(const size_t field) const {

	auto p = assignments.lower_bound(field);
	if (p != assignments.cend() && !assignments.key_comp()(field, p->first))
		return widgets[p->second].widget;
	
	return nullptr;
}

const IGridCellWidget *CMultipleCellWidget::GetCurrentWidget() const {

	return curr_widget;
}

size_t CMultipleCellWidget::GetWidgetsCount() const {

	return widgets_count;
}

size_t CMultipleCellWidget::GetAssignmentsCount() const {

	return assignments.size();
}
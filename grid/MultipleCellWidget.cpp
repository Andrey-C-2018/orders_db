#include "MultipleCellWidget.h"

CMultipleCellWidget::CMultipleCellWidget() { }

void CMultipleCellWidget::CreateCellWidget(XWindow *parent, const int flags, \
											const Tchar *label, \
											const int x, const int y, \
											const int width, const int height) {

	assert(!XEdit::IsCreated());
	XEdit::Create(parent, flags, label, x, y, width, height);

	for (auto &widget : widgets) {
		if (!widget.second.id != NOT_CREATED) {
			int widget_flags = flags;
			if (curr_field != widget.first)
				widget_flags &= (~FL_WINDOW_VISIBLE);

			widget.second.widget->CreateCellWidget(parent, widget_flags, label, x, y, width, height);
			widget.second.id = widget.second.widget->GetId();
		}
	}
}

void CMultipleCellWidget::SetOnChangeHandler(XEventHandlerData on_change) {

}

void CMultipleCellWidget::SetOnLooseFocusHandler(XEventHandlerData on_loose_focus) {

}

void CMultipleCellWidget::SetOnKeyPressHandler(XEventHandlerData on_key_press) {

}

void AddCellWidget(const size_t field, IGridCellWidget *widget) {

}

void CMultipleCellWidget::SetCurrentField(const size_t field) {

	if (field == curr_field) return;
	curr_field = field;

	IGridCellWidget *prev_widget = curr_widget;

	auto p = widgets.lower_bound(field);
	if (p != widgets.cend() && !widgets.key_comp()(field, p->first))
		curr_widget = p->second.widget;
	else 
		curr_widget = nullptr;

	if (curr_widget != prev_widget) {
		Hide(prev_widget);
		Show(curr_widget);
	}
}

void CMultipleCellWidget::Show() {

	if(curr_widget) curr_widget->Show();
	else XEdit::Show();
}

void CMultipleCellWidget::Hide() {

	if (curr_widget) curr_widget->Hide();
	else XEdit::Hide();
}

CMultipleCellWidget::~CMultipleCellWidget(){

	for (auto &widget : widgets)
		if(widget.second.id == NOT_CREATED)
			delete widget.second.widget;
}

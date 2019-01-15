#include "MultipleCellWidget.h"

CMultipleCellWidget::CMultipleCellWidget() : is_created(false) { }

void CMultipleCellWidget::SetCurrentField(const size_t field) {

	if (IsWidgetPresent(field)) curr_widget = widgets[field];
	else curr_widget = default_widget;
}

void CMultipleCellWidget::Show() {

	curr_widget->Show();
}

void CMultipleCellWidget::Hide() {

	curr_widget->Hide();
}

CMultipleCellWidget::~CMultipleCellWidget(){

	if (is_created) return;

	for (auto &widget : widgets)
		delete widget.second;
}
